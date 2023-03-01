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

global start_first_thread
global switch_context

extern scheduler_initialized
extern release_scheduler_lock

start_first_thread:
    ; get the thread's context
    mov    esp, [esp + 0x04]

    ; load registers
    pop edi
    pop esi
    pop ebx
    pop ebp

    mov dword [scheduler_initialized], 0x1
    call flush_tss
    call release_scheduler_lock

    ; start thread
    ret

switch_context:
    ; get both thread's contexts
    mov eax, [esp + 0x04]
    mov edx, [esp + 0x08]

    ; save current thread's context
    push ebp
    push ebx
    push esi
    push edi

    ; switch stacks
    mov [eax], esp
    mov esp, [edx]

    ; load next thread's context
    pop edi
    pop esi
    pop ebx
    pop ebp

    call release_scheduler_lock

    ; resume next thread
    ret

flush_tss:
    mov ax, 0x28
    ltr ax
    ret