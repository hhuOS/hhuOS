; Copyright (C) 2018 Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schoettner
; Heinrich-Heine University; Olaf Spinczyk, TU Dortmund
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

; all assembler code regarding interrupts is placed here
; i.e. wrapper bodies, bios-call code and IDT

; Original IDT and wrapper by by Olaf Spinczyk, TU Dortmund.
; Refactored and extended by
; Michael Schoettner, Fabian Ruhland, Filip Krakowski, Burak Akguel, Christian Gesse, HHU 2018

%include "kernel/core/constants.asm"

global bios_call
global setup_idt
global interrupt_return
global setSchedInit
global onException
global idt

extern preempt
extern dispatchInterrupt
extern switch_context
extern gdt_desc
extern gdt_bios_desc
extern BIOS_Page_Directory
extern stack
extern enable_interrupts
extern disable_interrupts


[SECTION .text]

; handle exceptions
onException:
    push ebp
    mov ebp, esp

    pushfd
    push cs
    push dword [ebp + 0x04]
    push 0x0
    push dword [ebp + 0x08]

    jmp wrapper_body


; procedure for bios_call
;
; procedure is splitted into different parts, because paging must be
; disabled
;
bios_call:
	; load bios-call IDT
    lidt    [idt16_descr]
	; safe registers
    pushfd
    pushad

	; check if scheduler is started (we have to switch the stack then,
	; because bios calls expext the stack to be placed at 4mb)
    mov ebx, [schedInit]
    cmp ebx, 0
    je skipStackSwitch

	; switch stack to startup stack used in startup.asm and save current stack pointer
	; this is necessary because this stack is used for the bios call without paging
	; and therefore we must know its physical address (which is currentStackAddress - 0xC0000000)
    mov ebx, stack
    add ebx, STACK_SIZE
    sub ebx, 4
    mov [ebx], esp
    mov esp, ebx

skipStackSwitch:
	; save address of current Page Directory
    mov ecx, cr3
    push ecx

	; enable 4mb-Paging
    mov ecx, cr4
    or ecx, 0x00000010
    mov cr4, ecx

	; load special 4mb-Page Directory for BIOS-calls
	; only important parts are mapped here
    mov ecx, BIOS_Page_Directory
	; get phys. address
    sub ecx, KERNEL_START
    mov cr3, ecx

	; jump to low address because paging will be disabled
	; kernel should be mapped at 0 and 3GB with BIOS-PD
	; necessary step: otherwise EIP points to wrong address
    lea ecx, [bios_call2]
    ; want to jump to low address
    sub ecx, KERNEL_START
    jmp ecx

bios_call2:
	; disable paging because we have to switch into real mode
    mov ecx, cr0
    and ecx, 0x7FFFFFFF
    mov cr0, ecx
	; flush TLB
    mov ecx, cr3
    mov cr3, ecx
	; load gdt for bios calls (-> low addresses / phys. address)
    lgdt [gdt_bios_desc - KERNEL_START]

	; for calculation
    mov edx, KERNEL_START
	; Shift values of some registers to low addresses because paging is disabled
    mov ecx, esp
    sub ecx, edx
    mov esp, ecx

    mov ecx, ebp
    sub ecx, edx
    mov ebp, ecx

bios_call3:
	; jump into BIOS-Segment
    call  0x18:0
    ; here we are back from 16-bit BIOS code
	; enable 4mb-Paging
    mov ecx, cr0
    or ecx, 0x80000000
    mov cr0, ecx
	; load global descriptor table
    lgdt [gdt_desc]
	; far jump to high address in kernel code (paging on)
    lea ecx, [bios_call4]
    jmp ecx

bios_call4:
	; shift values of some registers to high addresses for paging
    mov edx, KERNEL_START
    mov ecx, esp
    add ecx, edx
    mov esp, ecx

    mov ecx, ebp
    add ecx, edx
    mov ebp, ecx
	; load page table of process and enable 4kb paging
    pop ecx
    mov cr3, ecx
	; check if scheduler is active -> old stack has to be restored then
    mov ebx, [schedInit]
    cmp ebx, 0
    je skipStackSwitch2
	; restore old stack if necessary
    pop esp

skipStackSwitch2:
	; switch off 4mb Paging and enable 4kb paging
    mov ecx, cr4
    and ecx, 0xFFFFFFEF
    mov cr4, ecx
	; restore old register values
    popad
    popfd
	; load old IDT
    lidt	[idt_descr]
    ret

; is called when scheduler starts
setSchedInit:
    mov dword [schedInit], 0x1
    ret

;
; setup_idt
;
; Relocation of IDT-entries; set IDTR

setup_idt:
	mov	eax,wrapper_0	; ax: lower 16 Bit
	mov	ebx,eax
	shr	ebx,16      ; bx: upper 16 Bit
	mov	ecx,255     ; Counter
.loop:
    add	[idt+8*ecx+0],ax
	adc	[idt+8*ecx+6],bx
	dec	ecx
	jge	.loop

	lidt	[idt_descr]
	ret


; wrapper for interrupt-handling

%macro wrapper 1
wrapper_%1:

    push    0x00
    push    %1
    jmp	wrapper_body
%endmacro


; create first 14 wrappers
%assign i 0
%rep 14
wrapper i
%assign i i+1
%endrep

; Page-Fault wrapper is different, because error code ist pushed
wrapper_14:

    push    0x0E
    jmp	wrapper_body

; create all remaining wrappers
%assign i 15
%rep 241
wrapper i
%assign i i+1
%endrep

; unique body for all wrappers
wrapper_body:

    ; Save state
    pushad

    push    ds
    push    es
    push    fs
    push    gs

    cld

    ;save eax, as it may contain the system call number
    push    eax

    mov     ax, 0x10
    mov     ds, ax
    mov     es, ax
    mov     fs, ax
    mov     gs, ax

    ;restore eax
    pop eax

    ; Call interrupt handler
    push    esp
    call	dispatchInterrupt
    add     esp, 0x04

interrupt_return:

    ; Load new state
    pop     gs
    pop     fs
    pop     es
    pop     ds

    popad

    ; Remove error code and interrupt number
    add     esp, 0x08

    ; Set interrupt flag in EFLAGS
    ;or dword [esp + 0x08], 0x200

    iret




[SECTION .data]

idt_descr:
	dw	256*8-1     ; idt contains 256 entries
	dd	idt

;
; IDT for Realmode ;
; (Michael Schoettner)
;
idt16_descr:
    dw	1024    ; idt contains max. 1024 entries
    dd	0       ; address 0



;  create IDT with 256 entries

idt:

%macro idt_entry 1
	dw	(wrapper_%1 - wrapper_0) & 0xffff
	dw	0x0008
	dw	0x8e00
	dw	((wrapper_%1 - wrapper_0) & 0xffff0000) >> 16
%endmacro

; use macro

%assign i 0
%rep 256
idt_entry i
%assign i i+1
%endrep

; status of scheduler
schedInit:
    dw 0
