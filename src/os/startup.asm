; This is the entry-point for the whole system. The switch to protected mode
; is already done.
; Original by Olaf Spinczyk, TU Dortmund.
; Refactored and extended by
; Michael Schoettner, Fabian Ruhland, Filip Krakowski, Burak Akguel, Christian Gesse, HHU 2017

%include "kernel/constants.asm"

; Multiboot Constants
MULTIBOOT_PAGE_ALIGN	equ	1<<0
MULTIBOOT_MEMORY_INFO	equ	1<<1

; Multiboot Magic
MULTIBOOT_HEADER_MAGIC	equ	0x1badb002

; Multiboot Flags
MULTIBOOT_HEADER_FLAGS	equ	MULTIBOOT_PAGE_ALIGN | MULTIBOOT_MEMORY_INFO
MULTIBOOT_HEADER_CHKSUM	equ	-(MULTIBOOT_HEADER_MAGIC + MULTIBOOT_HEADER_FLAGS)

; Multiboot EAX Magic
MULTIBOOT_EAX_MAGIC	equ	0x2badb002

global startup
global on_paging_enabled
global __cxa_pure_virtual
global gdt_48
global _gdt_bios_48
global _init
global _fini
global stack

extern main
extern init_system
extern fini_system
extern setup_idt
extern paging_bootstrap
extern parse_multiboot

extern ___BSS_START__
extern ___BSS_END__
extern __init_array_start
extern __init_array_end
extern __fini_array_start
extern __fini_array_end

; calculate physical addresses for some labels
; needed if paging disabled, because functions are linked against high addresses
_gdt_bios_48		equ (gdt_bios_48 - KERNEL_START)
_gdt_bios			equ (gdt_bios - KERNEL_START)

	
[SECTION .text]

multiboot_header:
	align 4
	dd MULTIBOOT_HEADER_MAGIC
	dd MULTIBOOT_HEADER_FLAGS
	dd MULTIBOOT_HEADER_CHKSUM

startup:

    ; disable interrupts
    cli

	; load first 4MB-PageTable and enable paging
	lea ecx,  [paging_bootstrap - KERNEL_START]

	; jump into paging.asm to enable 4mb paging
	jmp ecx

on_paging_enabled:

; Load GDT
	lgdt	[gdt_48]

; global data segment
	mov	ax,0x10
	mov	ds,ax
	mov	es,ax
	mov	fs,ax
	mov	gs,ax
; make a jump to set the CS-register to the right value
; (the CodeSegment is placed at offset 0x8)
; if something at GDT is changed, this instruction may be changed as well
	jmp 0x8:clear_bss

; clean BSS
clear_bss:
    mov	edi, ___BSS_START__
.loop:
    mov	byte [edi], 0
    inc	edi
    cmp	edi, ___BSS_END__
    jne	.loop

; save multiboo structure address
    add ebx, KERNEL_START
    mov [multiboot_addr], ebx

; set init stack at 4mb
	mov	ss,ax
	mov esp, (stack + STACK_SIZE)

; setup interrupts
	call	setup_idt
	call	reprogram_pics

; Save multiboot structure address
    push dword [multiboot_addr]
    call parse_multiboot
    add  esp, 0x4

; call main in C-code
    call    init_system ; see SystemManagement.cc
	call	main	    ; call main in C/C++
	call	fini_system	; see SystemManagement.cc
	hlt

; Constructors of global objects
_init:
	mov	edi, __init_array_start
_init_loop:
	cmp	edi, __init_array_end
	je	_init_done
	mov	eax, [edi]
	call	eax
	add	edi, 4
	ja	_init_loop
_init_done:
	ret

; Destructors of global objects
_fini:
	mov	edi, __fini_array_start
_fini_loop:
	cmp	edi, __fini_array_end
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
; General global descriptor table
;
gdt:
	dw	0,0,0,0		; NULL Deskriptor

	dw	0xFFFF		; 4Gb - (0x100000*0x1000 = 4Gb)
	dw	0x0000		; base address=0
	dw	0x9A00		; code read/exec
	dw	0x00CF		; granularity=4096, 386 (+5th nibble of limit)

	dw	0xFFFF		; 4Gb - (0x100000*0x1000 = 4Gb)
	dw	0x0000		; base address=0
	dw	0x9200		; data read/write
	dw	0x00CF		; granularity=4096, 386 (+5th nibble of limit)

;
; global descriptor table for bios calls (now we have a bios segment at 0x24000)
;
gdt_bios:
	dw	0,0,0,0		; NULL Deskriptor

	dw	0xFFFF		; 4Gb - (0x100000*0x1000 = 4Gb)
	dw	0x0000		; base address=0
	dw	0x9A00		; code read/exec
	dw	0x00CF		; granularity=4096, 386 (+5th nibble of limit)

	dw	0xFFFF		; 4Gb - (0x100000*0x1000 = 4Gb)
	dw	0x0000		; base address=0
	dw	0x9200		; data read/write
	dw	0x00CF		; granularity=4096, 386 (+5th nibble of limit)

    dw  0xFFFF      ; 4Gb - (0x100000*0x1000 = 4Gb)
    dw	0x4000      ; 0x4000 -> base address=0x24000 (siehe BIOS.cc)
    dw  0x9A02      ; 0x2 -> base address =0x24000 (siehe BIOS.cc) und code read/exec;
    dw  0x008F      ; granularity=4096, 16-bit code

; value for GDTR 
gdt_48:
	dw	0x18		; GDT Limit=32, 4 GDT Eintraege
	dd	gdt         ; Virtuelle Adresse der GDT

; value for GDTR using BIOS Calls (low address needed because paging is disabled)
gdt_bios_48:
	dw	0x20		; GDT Limit=32, 4 GDT Eintraege
	dd	_gdt_bios         ; Virtuelle Adresse der GDT

multiboot_addr:
    dd  0x00000000

section .bss
align 32
stack:
    resb STACK_SIZE




