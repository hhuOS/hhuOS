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

; Contains the code, that is needed to perform the switch to real and issue a bios call.

%include "constants.asm"

; Export functions
global bios_call
global set_scheduler_initialized

; Import variables
extern initial_kernel_stack
extern bios_page_directory
extern gdt_bios_descriptor
extern gdt_descriptor
extern idt_descriptor

; Perform a bios call in real mode
; The procedure is split into different parts, because paging need to be disabled
bios_call:
	; Load bios call IDT
    lidt    [real_mode_idt_descriptor]
	; Safe registers
    pushfd
    pushad

	; Check if scheduler is started (we have to switch the stack then,
	; because bios calls expect the stack to be placed at 4MB)
    mov ebx, [scheduler_initialized]
    cmp ebx, 0
    je  skip_stack_switch

	; Switch stack to boot stack used in boot.asm and save current stack pointer
	; This is necessary because this stack is used for the bios call without paging
	; and therefore we must know its physical address (which is current stack address - 0xC0000000)
    mov ebx, initial_kernel_stack
    add ebx, (STACK_SIZE - 4)
    mov [ebx], esp
    mov esp, ebx

skip_stack_switch:
	; Save address of current Page Directory
    mov  ecx, cr3
    push ecx

	; Enable 4MB paging
    mov ecx, cr4
    or  ecx, 0x00000010
    mov cr4, ecx

	; Load special 4MB page directory for bios calls
	; Only important parts are mapped here
	mov ecx, (bios_page_directory - KERNEL_START)
    mov cr3, ecx

	; Jump to low address because paging will be disabled
	; The kernel should be mapped at 0 and 3GB with bios page directory
	; This is a necessary step, otherwise the eip points to the wrong address
    lea ecx, [bios_call_2 - KERNEL_START]
    jmp ecx

bios_call_2:
	; Disable paging because we have to switch into real mode
    mov ecx, cr0
    and ecx, 0x7FFFFFFF
    mov cr0, ecx
	; Flush TLB
    mov ecx, cr3
    mov cr3, ecx
	; Load gdt for bios calls (with low/physical addresses)
    lgdt [gdt_bios_descriptor - KERNEL_START]

	; Shift values of esp and ebp to low addresses, because paging is disabled
    mov ecx, esp
    sub ecx, KERNEL_START
    mov esp, ecx

    mov ecx, ebp
    sub ecx, KERNEL_START
    mov ebp, ecx

	; Jump into bios segment
    call  0x18:0

    ; We return here from 16-bit bios code
	; Enable 4MB-Paging
    mov ecx, cr0
    or  ecx, 0x80000000
    mov cr0, ecx
	; Load global descriptor table
    lgdt [gdt_descriptor]
	; Perform a far jump to a high address in kernel code (paging enabled)
    lea ecx, [bios_call_3]
    jmp ecx

bios_call_3:
	; Shift values of esp and ebp to high addresses, because paging is enabled
    mov ecx, esp
    add ecx, KERNEL_START
    mov esp, ecx

    mov ecx, ebp
    add ecx, KERNEL_START
    mov ebp, ecx

	; Load page table of process and enable 4KB paging
    pop ecx
    mov cr3, ecx

	; Check if scheduler is active -> old stack has to be restored then
    mov ebx, [scheduler_initialized]
    cmp ebx, 0
    je  skip_stack_switch_2
	; Restore old stack if necessary
    pop esp

skip_stack_switch_2:
	; Switch off 4MB Paging and enable 4KB paging
    mov ecx, cr4
    and ecx, 0xFFFFFFEF
    mov cr4, ecx
	; Restore old register values
    popad
    popfd
	; Load old IDT
    lidt	[idt_descriptor]
    ret


; Is called when scheduler starts
set_scheduler_initialized:
    mov dword [scheduler_initialized], 0x1
    ret

; IDT for real mode
real_mode_idt_descriptor:
    dw	1024 ; idt contains max. 1024 entries
    dd	0    ; address 0

; Indicates whether the scheduler has been started
scheduler_initialized:
    dw 0