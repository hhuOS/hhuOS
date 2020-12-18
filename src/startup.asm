; Copyright (C) 2018 Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schoettner
; Heinrich-Heine University
;
; This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public
; License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any
; later version.
;
; This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied
; warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
; details.
;
; You should have received a copy of the GNU General Public License
; along with this program.  If not, see <http://www.gnu.org/licenses/>

; This is the entry-point for the whole system. The switch to protected mode is already done.
;
; Original by Olaf Spinczyk, TU Dortmund.
; Refactored and extended by
; Michael Schoettner, Fabian Ruhland, Filip Krakowski, Burak Akguel, Christian Gesse, HHU 2017

%include "kernel/core/constants.asm"

; Multiboot Constants
MULTIBOOT_PAGE_ALIGN	equ	1<<0
MULTIBOOT_MEMORY_INFO	equ	1<<1
MULTIBOOT_GRAPHICS_INFO equ 1<<2
MULTIBOOT_ADDRESS_INFO  equ 1<<16

; Multiboot Magic
MULTIBOOT_HEADER_MAGIC	equ	0x1badb002
MUTLIBOOT_EAX_MAGIC     equ 0x2badb002

; Multiboot Flags
MULTIBOOT_HEADER_FLAGS	equ	MULTIBOOT_PAGE_ALIGN | MULTIBOOT_MEMORY_INFO | MULTIBOOT_GRAPHICS_INFO
MULTIBOOT_HEADER_CHKSUM	equ	-(MULTIBOOT_HEADER_MAGIC + MULTIBOOT_HEADER_FLAGS)

; Multiboot EAX Magic
MULTIBOOT_EAX_MAGIC	equ	0x2badb002

; Multiboot options
GRAPHICS_MODE   equ 0
GRAPHICS_WIDTH  equ 800
GRAPHICS_HEIGHT equ 600
GRAPHICS_BPP    equ 32

; make labels global
global startup
global on_paging_enabled
global __cxa_pure_virtual
global gdt_desc
global gdt_bios_desc
global _init
global _fini
global stack

; functions and labels from different sources
extern main
extern init_system
extern fini_system
extern setup_idt
extern paging_bootstrap
extern enable_interrupts
extern copyMultibootInfo
extern readMemoryMap
extern init_gdt

extern ___KERNEL_DATA_START__
extern ___KERNEL_DATA_END__
extern ___BSS_START__
extern ___BSS_END__
extern ___INIT_ARRAY_START__
extern ___INIT_ARRAY_END__
extern ___FINI_ARRAY_START__
extern ___FINI_ARRAY_END__
extern ___TEXT_START__
extern ___TEXT_END__


; calculate physical addresses for some labels
; needed if paging disabled, because functions are linked against high addresses
_clear_bss              equ (clear_bss - KERNEL_START)
___PHYS_BSS_START__     equ (___BSS_START__ - KERNEL_START)
___PHYS_BSS_END__       equ (___BSS_END__ - KERNEL_START)

	
section .text

multiboot_header:
	align 4
	dd MULTIBOOT_HEADER_MAGIC
	dd MULTIBOOT_HEADER_FLAGS
	dd MULTIBOOT_HEADER_CHKSUM
	dd (multiboot_header    - KERNEL_START)
	dd (___KERNEL_DATA_START__   - KERNEL_START)
	dd (___KERNEL_DATA_END__     - KERNEL_START)
	dd (___BSS_END__        - KERNEL_START)
	dd (startup             - KERNEL_START)
	dd GRAPHICS_MODE
	dd GRAPHICS_WIDTH
	dd GRAPHICS_HEIGHT
	dd GRAPHICS_BPP

startup:

    ; enable SSE
    mov eax, cr0
    and ax, 0xFFFB
    or ax, 0x2
    mov cr0, eax
    mov eax, cr4
    or ax, 3 << 9
    mov cr4, eax

	; set esp for following function call
    mov esp, (stack - KERNEL_START + STACK_SIZE)

	; push parameters to set up GDTs
    mov ecx, gdt_phys_desc
    sub ecx, KERNEL_START
    push ecx

    mov ecx, gdt_bios_desc
    sub ecx, KERNEL_START
    push ecx

    mov ecx, gdt_desc
    sub ecx, KERNEL_START
    push ecx

    mov ecx, gdt_bios
    sub ecx, KERNEL_START
    push ecx

    mov ecx, gdt
    sub ecx, KERNEL_START
    push ecx

	; set up GDTs
    call init_gdt

	; clean up stack
    add esp, 0x14

    ; load GDT from physical address
    lgdt	[gdt_phys_desc - KERNEL_START]

	; set segment-registers
    mov	ax,0x10
    mov	ds,ax
    mov	es,ax
    mov	fs,ax
    mov	gs,ax
    mov	ss,ax
    ; invoke a jump to set the CS-register to the right value
    ; (the CodeSegment is placed at offset 0x8)
    ; if something at GDT is changed, this instruction may be changed as well
    jmp 0x8:_clear_bss

    ; clean BSS
clear_bss:
    mov	edi, ___PHYS_BSS_START__
.loop:
    mov	byte [edi], 0
    inc	edi
    cmp	edi, ___PHYS_BSS_END__
    jne	.loop

    ; set stack again to cut off possible old values
    mov esp, (stack - KERNEL_START + STACK_SIZE)
    
    ; copy the Multiboot info struct recursively
    mov eax, multiboot_data
    sub eax, KERNEL_START
    push eax
    push ebx
    mov eax, copyMultibootInfo
    sub eax, KERNEL_START
    call eax
    add esp, 0x04
    mov ebx, 0
    
    ; read memory map and push parameters before
    mov eax, multiboot_data
    sub eax, KERNEL_START
    push eax
    mov eax, readMemoryMap
    sub eax, KERNEL_START
    call eax
    add esp, 0x04

	; load label for bootstrapping the paging system
	mov ecx,  (paging_bootstrap - KERNEL_START)
	; jump into paging.asm to enable 4mb paging
	jmp ecx

; here we come back from paging.asm after 4mb paging is enabled
on_paging_enabled:
    ; load GDT from virtual address
	lgdt	[gdt_desc]

    ; save multiboot structure address
    add ebx, KERNEL_START
    mov [multiboot_addr], ebx

    ; set init stack
	mov esp, (stack + STACK_SIZE)


    ; setup interrupts (see interrupts.asm)
	call	setup_idt
	call	reprogram_pics

    ; initialize system
    push multiboot_data
    ; call to SystemManagement.cpp
    call    init_system
    add  esp, 0x4

    ; call kernel's main() function
	call	main

    ; finalize system
	call	fini_system	; see SystemManagement.cpp
	hlt

; Constructors of global objects
_init:
	mov	edi, ___INIT_ARRAY_START__
_init_loop:
	cmp	edi, ___INIT_ARRAY_END__
	je	_init_done
	mov	eax, [edi]
	call	eax
	add	edi, 4
	ja	_init_loop
_init_done:
	ret

; Destructors of global objects
_fini:
	mov	edi, ___FINI_ARRAY_START__
_fini_loop:
	cmp	edi, ___FINI_ARRAY_END__
	je	_fini_done
	mov	eax, [edi]
	call	eax
	add	edi, 4
	ja	_fini_loop
_fini_done:
	ret

;
; reprogram_pics
;
; Reprogram PICs - all 15 HW-Interrupts are one after each other in IDT

reprogram_pics:	
	mov	al,0x11     ; ICW1: 8086 Mode with ICW4
	out	0x20,al   
	call	delay
	out	0xa0,al
	call	delay
	mov	al,0x20     ; ICW2 Master: IRQ # Offset (32)
	out	0x21,al
	call	delay
	mov	al,0x28     ; ICW2 Slave: IRQ # Offset (40)
	out	0xa1,al
	call	delay
	mov	al,0x04     ; ICW3 Master: Slaves at IRQs
	out	0x21,al
	call	delay
	mov	al,0x02     ; ICW3 Slave: Connected with IRQ2 of master
	out	0xa1,al
    call	delay
	mov	al,0x07     ; ICW4 Master: 8086 Mode and automatic EIO
	out	0x21,al
	call	delay
	mov al,0x03     ; ICW4 Slave: 8086 Mode and automatic EIO
	out	0xa1,al
	call	delay

	mov	al,0xff     ; Mask HW-Interrupts from PICs
	out	0xa1,al     ; Use only interrupt 2 to cascade the PICs
    call	delay
	mov	al,0xfb
	out	0x21,al

	ret

; delay
;
delay:
	jmp	.L2
.L2:	ret

; This function is used when global Constructors are called.
; The label must be defined but can be void

__cxa_pure_virtual:
; Michael Schoettner:
; This label replaces delete, which is implemendet right now.
; We can comment it out.
;_ZdlPv:
	ret


section .data
;
; the following lines reserve memory for the different GDTs needed in hhuOS.
; these GDTs and descriptors are set up in init_gdt (SystemManagement.cpp)

;global descriptor table
gdt:
	times (24) dw 0

; global descriptor table for bios calls
gdt_bios:
	times (16) dw 0

; descriptor for GDT
gdt_desc:
	dw	0		                   ; GDT limit
	dd	0                          ; virtual address of GDT

; physical descriptor for gdt (needed if paging disabled)
gdt_phys_desc:
	dw	0		                   ; GDT limit
	dd	0                          ; Physical GDT address

; descriptor for BIOS Call GDT
gdt_bios_desc:
	dw	0						   ; GDT limit
	dd	0         				   ; physical BIOS-GDT address

multiboot_addr:
    dd  0x00000000


; reserve space for initial kernel stack
section .bss
align 32
stack:
    resb STACK_SIZE

; reserve space for a copy of the Multiboot information
multiboot_data:
    resb MULTIBOOT_SIZE
