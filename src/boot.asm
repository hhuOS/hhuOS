; Copyright (C) 2018-2022 Heinrich-Heine-Universitaet Duesseldorf,
; Institute of Computer Science, Department Operating Systems
; Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schoettner
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
; Original by Olaf Spinczyk, TU Dortmund
; Refactored and extended by Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schoettner, HHU

%include "constants.asm"

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

; Multiboot options
GRAPHICS_MODE   equ 0
GRAPHICS_WIDTH  equ 800
GRAPHICS_HEIGHT equ 600
GRAPHICS_BPP    equ 32

; Export variables
global initial_kernel_stack
global gdt_descriptor
global gdt_bios_descriptor
extern multiboot_data
extern acpi_data

; Export functions
global boot
global on_paging_enabled
global _init
global _fini
global __cxa_pure_virtual

; Import functions
extern main
extern init_gdt
extern copy_multiboot_info
extern copy_acpi_tables
extern read_memory_map
extern initialize_system
extern finish_system
extern setup_idt
extern reprogram_pics
extern enable_bootstrap_paging
extern enable_interrupts

; Import linker symbols
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

; Calculate physical addresses for some labels
; Needed while paging is disabled, because functions are linked against high addresses
_clear_bss          equ (clear_bss - KERNEL_START)
___PHYS_BSS_START__ equ (___BSS_START__ - KERNEL_START)
___PHYS_BSS_END__   equ (___BSS_END__ - KERNEL_START)

section .text

multiboot_header:
	align 4
	dd MULTIBOOT_HEADER_MAGIC
	dd MULTIBOOT_HEADER_FLAGS
	dd MULTIBOOT_HEADER_CHKSUM
	dd (multiboot_header - KERNEL_START)
	dd (___KERNEL_DATA_START__ - KERNEL_START)
	dd (___KERNEL_DATA_END__ - KERNEL_START)
	dd (___BSS_END__ - KERNEL_START)
	dd (boot - KERNEL_START)
	dd GRAPHICS_MODE
	dd GRAPHICS_WIDTH
	dd GRAPHICS_HEIGHT
	dd GRAPHICS_BPP

boot:
    ; Save multiboot structure address
    mov [multiboot_physical_addr - KERNEL_START], ebx

	; Set esp to initial kernel stack
    mov esp, (initial_kernel_stack - KERNEL_START + STACK_SIZE)

	; Setup global descriptor tables
    push dword (gdt_phys_descriptor - KERNEL_START)
    push dword (gdt_bios_descriptor - KERNEL_START)
    push dword (gdt_descriptor - KERNEL_START)
    push dword (gdt_bios - KERNEL_START)
    push dword (gdt - KERNEL_START)
    call_physical_function init_gdt

    ; Load GDT from physical address
    lgdt [gdt_phys_descriptor - KERNEL_START]

	; Set segment-registers
    mov	ax,0x10
    mov	ds,ax
    mov	es,ax
    mov	fs,ax
    mov	gs,ax
    mov	ss,ax
    ; Invoke a jump to set the CS-register to the right value (the code segment is placed at offset 0x8)
    ; If something at the GDT is changed, this instruction may need to be changed as well
    jmp 0x8:_clear_bss

; Zero out bss
clear_bss:
    mov	edi,___PHYS_BSS_START__
clear_bss_loop:
    cmp	edi,___PHYS_BSS_END__
    jge clear_bss_done
    mov	byte [edi],0
    inc	edi
    jmp clear_bss_loop
clear_bss_done:
    ; Set stack again to cut off possible old values
    mov esp, (initial_kernel_stack - KERNEL_START + STACK_SIZE)

    ; Copy the multiboot info struct into bss
    push dword MULTIBOOT_SIZE
    push dword (multiboot_data - KERNEL_START)
    push dword [multiboot_physical_addr - KERNEL_START]
    call_physical_function copy_multiboot_info

    ; Copy the ACPI structures into bss
    push dword ACPI_SIZE
    push dword (acpi_data - KERNEL_START)
    call_physical_function copy_acpi_tables

    ; Read memory map from multiboot info struct
    push dword [multiboot_physical_addr - KERNEL_START]
    call_physical_function read_memory_map

	; Jump into paging.asm to enable 4MB paging
	call_physical_function enable_bootstrap_paging

; We return here from paging.asm after 4MB paging is enabled
on_paging_enabled:
    ; Load GDT from virtual address
	lgdt [gdt_descriptor]

    ; Set esp to initial kernel stack
	mov esp, (initial_kernel_stack + STACK_SIZE)

    ; Setup interrupts (see interrupts.asm)
	call setup_idt
	call reprogram_pics

    ; Initialize system
    call initialize_system

    ; Call the kernel's main() function
	call main

    ; Finalize system
	call finish_system
	hlt

; Call constructors of global objects
_init:
	mov edi,___INIT_ARRAY_START__
_init_loop:
	cmp edi,___INIT_ARRAY_END__
	jge _init_done
	call [edi]
	add	edi,4
	jmp _init_loop
_init_done:
	ret

; Call destructors of global objects
_fini:
    mov	 edi,___FINI_ARRAY_START__
_fini_loop:
    cmp	 edi,___FINI_ARRAY_END__
    jge _fini_done
    call [edi]
    add	 edi,4
    jmp _fini_loop
_fini_done:
    ret

; This function is used when global constructors are called
; The label must be defined but can be void
__cxa_pure_virtual:
	ret

section .data

; The following lines reserve memory for the different GDTs needed in hhuOS
; These GDTs and descriptors are set up in init_gdt (SystemManagement.cpp)

; Global descriptor table
gdt:
	times (24) dw 0

; Global descriptor table for bios calls
gdt_bios:
	times (16) dw 0

; Descriptor for gdt
gdt_descriptor:
	dw	0		                   ; GDT limit
	dd	0                          ; virtual address of GDT

; Physical descriptor for gdt (needed if paging disabled)
gdt_phys_descriptor:
	dw	0		                   ; GDT limit
	dd	0                          ; Physical GDT address

; Descriptor for bios call gdt
gdt_bios_descriptor:
	dw	0						   ; GDT limit
	dd	0         				   ; physical BIOS-GDT address

multiboot_physical_addr:
    dd  0

section .bss

; Reserve space for initial kernel stack
align 32
initial_kernel_stack:
    resb STACK_SIZE

; Reserve space for a copy of the multiboot information
multiboot_data:
    resb MULTIBOOT_SIZE

; Reserve space for a copy of the ACPI tables
acpi_data:
    resb ACPI_SIZE
