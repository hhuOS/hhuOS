; Copyright (C) 2018-2024 Heinrich-Heine-Universitaet Duesseldorf,
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

global start_kernel_thread
global start_user_thread
global switch_thread

extern set_tss_stack_entry
extern release_scheduler_lock

%define PUSHAD_STACK_SPACE 8 * 4
%define PUSHF_STACK_SPACE 1 * 4
%define PUSH_SEGMENT_REGISTERS_SPACE 4 * 4

start_kernel_thread:
    mov esp, [esp + 4] ; First parameter -> load 'oldStackPointer'

    ; Load registers from prepared stack
    pop ds
    pop es
    pop fs
    pop gs
    popfd
    popad

    call release_scheduler_lock

    ret

start_user_thread:
    mov esp, [esp + 4] ; First parameter -> load 'oldStackPointer'
    iret ; Switch to user mode

switch_thread:
    ; Save registers of current thread
    pushad
    pushfd
    push gs
    push fs
    push es
    push ds

    ; Save stack pointer in first parameter 'currentStackPointer'
    mov eax, [esp + PUSHAD_STACK_SPACE + PUSHF_STACK_SPACE + PUSH_SEGMENT_REGISTERS_SPACE + 4]
    mov [eax], esp

    ; Set TSS stack entry using third parameter 'nextStackEndPointer'
    push dword [esp + PUSHAD_STACK_SPACE + PUSHF_STACK_SPACE + PUSH_SEGMENT_REGISTERS_SPACE + 12]
    call set_tss_stack_entry
    add esp, 4

    ; Load registers of next thread using second parameter 'nextStackPointer'
    mov esp, [esp + PUSHAD_STACK_SPACE + PUSHF_STACK_SPACE + PUSH_SEGMENT_REGISTERS_SPACE + 8]
    pop ds
    pop es
    pop fs
    pop gs
    popfd
    popad

    call release_scheduler_lock
    ret

flush_tss:
    mov ax, 0x28
    ltr ax
    ret