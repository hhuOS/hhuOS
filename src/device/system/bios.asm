; Copyright (C) 2017-2026 Heinrich Heine University Düsseldorf,
; Institute of Computer Science, Department Operating Systems
; Main developers: Christian Gesse <christian.gesse@hhu.de>, Fabian Ruhland <ruhland@hhu.de>
; Original development team: Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schöttner
; This project has been supported by several students.
; A full list of integrated student theses can be found here: https://github.com/hhuOS/hhuOS/wiki/Student-theses
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

[GLOBAL protected_mode_call]
[GLOBAL real_mode_call]
[GLOBAL bios_call_16_start]
[GLOBAL bios_call_16_interrupt]
[GLOBAL bios_call_16_end]

BIOS_CALL_CODE equ 0x00000500
BIOS_CALL_STACK equ 0x00000800

[SECTION .text]
[BITS 32]

; Execute a protected mode BIOS call (Using 0x500 as entry point allows executing real mode BIOS calls)
protected_mode_call:
    ; Store registers
    pushad
    pushfd

    ; Switch stack to prepared stack for BIOS call
    mov [esp_backup],esp ; Store ESP
    mov edx,[(esp + 36) + 8] ; Store second parameter (entry point) in EDX
    mov esp,[(esp + 36) + 4] ; First parameter (Pointer to prepared stack for BIOS call)
    push edx ; Store entry point on new stack

    ; Pop parameters from the struct (now on stack) into the CPU registers
    add esp,4 ; Do not pop entry point
    pop ds
    pop es
    pop fs
    pop gs
    pop eax ; Only to skip flags
    popad

    ; Disable paging and backup CR3
    push eax
    mov eax,cr0
    and eax,0x7fffffff
    mov cr0,eax
    mov eax,cr3
    mov [cr3_backup],eax
    pop eax

    ; Execute call
    push 0x08 ; Kernel code segment
    push protected_mode_call_return ; Next instruction after BIOS call
    push 0x18 ; Code segment for BIOS call
    push dword [esp - 44] ; Entry point for BIOS function
    retf ; Call BIOS function

; We return here from BIOS code
protected_mode_call_return:
    ; Push registers into the parameter struct, which then holds return values from the BIOS call
    pushad
    pushfd ; Do not skip flags this time
    push gs
    push fs
    push es
    push ds

    ; Restore CR3
    mov eax,[cr3_backup]
    mov cr3,eax

    ; Enable paging
    mov eax,cr0
    or eax,0x80000000
    mov cr0,eax

    ; Restore esp
    mov esp,[esp_backup]

    ; Restore registers
    popfd
    popad

    ret

real_mode_call:
    ; Store registers
    pushad
    pushfd

    ; Switch stack to prepared stack for BIOS call
    mov [esp_backup],esp ; Store ESP
    mov esp,[(esp + 36) + 4] ; First parameter (Pointer to prepared stack for BIOS call)

    ; Disable paging and backup CR3
    mov eax,cr0
    and eax,0x7fffffff
    mov cr0,eax
    mov eax,cr3
    mov [cr3_backup],eax

    call 0x18:BIOS_CALL_CODE

; We return here from BIOS code
real_mode_call_return:
    ; Restore CR3
    mov eax,[cr3_backup]
    mov cr3,eax

    ; Enable paging
    mov eax,cr0
    or eax,0x80000000
    mov cr0,eax

    ; Restore esp
    mov esp,[esp_backup]

    ; Restore registers
    popfd
    popad

    ret

[BITS 16]
; Leave protected mode and call BIOS function via interrupt in real mode
bios_call_16_start:
    ; Load data segment
    mov dx,0x20
    mov ds,dx
    mov es,dx
    mov fs,dx
    mov gs,dx
    mov ss,dx

    ; Turn of protected mode via cr0 (also disable write protection)
    mov eax,cr0
    and eax,0x7ffefffe
    mov cr0,eax

    ; Flush pipeline and switch decoding unit to real mode by performing a far jump to the next instruction
    jmp 0x0000:(BIOS_CALL_CODE + (bios_call_16_real_mode_enter - bios_call_16_start))

bios_call_16_real_mode_enter:
    ; Setup segment registers for real mode
    mov dx,0x0000
    mov ds,dx
    mov es,dx
    mov fs,dx
    mov gs,dx
    mov ss,dx

    ; Skip return address and segment
    add esp,8

    ; Pop parameters from the struct (on stack) into the CPU registers
    pop ds
    pop es
    pop fs
    pop gs
    pop ax ; Only to skip flags
    popad

    ; Make sure that the BIOS call will not overwrite the return address and segment
    sub esp,50

; Perform BIOS call (value is manually placed into memory by Bios::interrupt())
bios_call_16_interrupt:
    int 0x00

    ; Disable interrupts (might have been enabled by the BIOS)
    cli

    ; Store flags (might be affected by the next instruction)
    pushf

    ; Restore ESP
    add esp,50 + 2

    ; Push registers into the parameter struct, which then holds return values from the BIOS call
    pushad
    push word [esp - 50 + (32 - 2)] ; Push stored flags
    push gs
    push fs
    push es
    push ds

    ; Let stack point to return address and segment
    sub esp,8

    ; Enable protected mode (without paging) and write protection
    mov eax,cr0
    or eax,0x00010001
    mov cr0,eax

    ; Flush pipeline and switch decoding unit to protected mode by performing a far jump to the next instruction
    jmp 0x0018:(BIOS_CALL_CODE + (bios_call_16_real_mode_leave - bios_call_16_start))

bios_call_16_real_mode_leave:
    ; Setup segment registers for protected mode
    mov dx,0x0010
    mov ds,dx
    mov es,dx
    mov fs,dx
    mov gs,dx
    mov ss,dx

    ; Far return to real_mode_call_return:
[BITS 32]
    retfw
bios_call_16_end:

[SECTION .data]
esp_backup dd 0x00000000
cr3_backup dd 0x00000000