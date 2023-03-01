; Copyright (C) 2018-2023 Heinrich-Heine-Universitaet Duesseldorf,
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
global reprogram_pics
global interrupt_return
global on_exception

; Export variables
global idt_descriptor

; Import functions
extern dispatch_interrupt
extern enable_interrupts
extern disable_interrupts
extern set_tss_stack_entry

section .text

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
    mov    eax,wrapper_0 ; ax: lower 16 Bit
    mov    ebx,eax
    shr    ebx,16        ; bx: upper 16 Bit
    mov    ecx,255       ; counter
.loop:
    add    [idt + 8 * ecx + 0],ax
    adc    [idt + 8 * ecx + 6],bx
    dec    ecx
    jge    .loop

    lidt [idt_descriptor]
    ret

; Reprogram PICs: All 15 hardware interrupts are one after each other in the IDT
reprogram_pics:
    mov     al,0x11     ; ICW1: 8086 Mode with ICW4
    out     0x20,al
    call delay
    out     0xa0,al
    call delay
    mov     al,0x20     ; ICW2 Master: IRQ # Offset (32)
    out     0x21,al
    call delay
    mov     al,0x28     ; ICW2 Slave: IRQ # Offset (40)
    out     0xa1,al
    call delay
    mov     al,0x04     ; ICW3 Master: Slaves at IRQs
    out     0x21,al
    call delay
    mov     al,0x02     ; ICW3 Slave: Connected with IRQ2 of master
    out     0xa1,al
    call delay
    mov     al,0x01     ; ICW4 Master: 8086 Mode
    out     0x21,al
    call delay
    mov  al,0x01     ; ICW4 Slave: 8086 Mode
    out     0xa1,al
    call delay

    mov     al,0xff     ; Mask HW-Interrupts from PICs
    out     0xa1,al     ; Use only interrupt 2 to cascade the PICs
    call delay
    mov     al,0xfb
    out     0x21,al

    ret

; Delay function
delay:
    jmp    .L2
.L2:
    ret

; Wrapper for interrupt handling
%macro wrapper 1
wrapper_%1:
    push 0x00
    push %1
    jmp     wrapper_body
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
    jmp    wrapper_body

; Page fault wrapper is different, because error code is pushed automatically
wrapper_14:
    push 0x0E
    jmp    wrapper_body

; Create all remaining wrappers
%assign i 15
%rep 241
wrapper i
%assign i i + 1
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

section .data

idt_descriptor:
    dw    256 * 8 - 1 ; idt contains 256 entries
    dd    idt

; Create IDT with 256 entries
idt:
%macro idt_entry 1
    dw    (wrapper_%1 - wrapper_0) & 0xffff
    dw    0x0008
    dw    0xee00
    dw    ((wrapper_%1 - wrapper_0) & 0xffff0000) >> 16
%endmacro

; Create first 134 entries
%assign i 0
%rep 134
idt_entry i
%assign i i + 1
%endrep

; Create system call entry
dw (wrapper_134 - wrapper_0) & 0xffff
dw    0x0008
dw    0xee00
dw ((wrapper_134 - wrapper_0) & 0xffff) >> 16

; Create remaining entries
%assign i 135
%rep 121
idt_entry i
%assign i i + 1
%endrep