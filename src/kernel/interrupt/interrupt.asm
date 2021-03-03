; Copyright (C) 2018-2021 Heinrich-Heine-Universitaet Duesseldorf,
; Institute of Computer Science, Department Operating Systems
; Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schoettner; Olaf Spinczyk, TU Dortmund
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

; All assembler code regarding interrupts is placed here (i.e. wrapper bodies and IDT).
; Original IDT and wrapper by by Olaf Spinczyk, TU Dortmund
; Refactored and extended by Michael Schoettner, Fabian Ruhland, Filip Krakowski, Burak Akguel, Christian Gesse, HHU 2018

%include "constants.asm"

; Export functions
global setup_idt
global interrupt_return
global on_exception

; Export variables
global idt_descriptor

; Import functions
extern dispatch_interrupt
extern enable_interrupts
extern disable_interrupts
extern set_tss_stack_entry

[SECTION .text]

; Handle exceptions
on_exception:
    push ebp
    mov  ebp, esp

    pushfd
    push cs
    push dword [ebp + 0x04]
    push 0x0
    push dword [ebp + 0x08]
    jmp  wrapper_body

; Relocation of IDT-entries; set IDTR
setup_idt:
	mov	eax,wrapper_0 ; ax: lower 16 Bit
	mov	ebx,eax
	shr	ebx,16        ; bx: upper 16 Bit
	mov	ecx,255       ; counter
.loop:
    add	[idt+8*ecx+0],ax
	adc	[idt+8*ecx+6],bx
	dec	ecx
	jge	.loop

	lidt [idt_descriptor]
	ret

; Wrapper for interrupt handling
%macro wrapper 1
wrapper_%1:
    push 0x00
    push %1
    jmp	 wrapper_body
%endmacro

; Create first 13 wrappers
%assign i 0
%rep 13
wrapper i
%assign i i+1
%endrep

; General protection fault wrapper is different, because error code is pushed automatically
wrapper_13:
    push 0x0D
    jmp	wrapper_body

; Page fault wrapper is different, because error code is pushed automatically
wrapper_14:
    push 0x0E
    jmp	wrapper_body

; Create all remaining wrappers
%assign i 15
%rep 241
wrapper i
%assign i i+1
%endrep

; Unique body for all wrappers
wrapper_body:
    ; Save state
    pushad
    push ds
    push es
    push fs
    push gs
    cld

    ; Save eax, as it may contain the system call number
    push eax

    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    ; Restore eax
    pop eax

    ; Call interrupt handler
    push esp
    call dispatch_interrupt
    add  esp, 0x04

; Newly created threads start here
interrupt_return:
    ; Set TSS to current kernel stack
    push esp
    call set_tss_stack_entry
    add  esp, 0x04

    ; Load new state
    pop gs
    pop fs
    pop es
    pop ds
    popad

    ; Remove error code and interrupt number
    add esp, 0x08
    iret

[SECTION .data]

idt_descriptor:
	dw	256*8-1 ; idt contains 256 entries
	dd	idt

; Create IDT with 256 entries
idt:
%macro idt_entry 1
	dw	(wrapper_%1 - wrapper_0) & 0xffff
	dw	0x0008
	dw	0x8e00
	dw	((wrapper_%1 - wrapper_0) & 0xffff0000) >> 16
%endmacro

%assign i 0
%rep 256
idt_entry i
%assign i i+1
%endrep
