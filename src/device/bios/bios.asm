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

; Contains the code, that is needed to perform the switch to real and issue a bios call.

%include "constants.asm"

; Export functions
global bios_call
global bios_call_16_start
global bios_call_16_end
global bios_call_16_interrupt

; Export variables
global scheduler_initialized

; Import variables
extern initial_kernel_stack
extern bios_page_directory
extern gdt_bios_descriptor
extern gdt_descriptor
extern idt_descriptor

section .text
bits 32

; Perform a bios call in real mode
; The procedure is split into different parts, because paging need to be disabled
bios_call:
    ; Safe registers
    pushfd
    pushad

    ; Check if the scheduler is running (we have to switch the stack then,
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
    and ecx, 0x7fffffff
    mov cr0, ecx
    ; Flush TLB
    mov ecx, 0
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
    call  0x18:0x4000

bios_call_16_return:
    ; We return here from 16-bit bios code
    ; Enable 4MB-Paging
    mov ecx, (bios_page_directory - KERNEL_START)
    mov cr3, ecx

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

    ; Check if scheduler is running -> old stack has to be restored then
    mov ebx, [scheduler_initialized]
    cmp ebx, 0
    je  skip_stack_switch_2
    ; Restore old stack if necessary
    pop esp

skip_stack_switch_2:
    ; Switch off 4MB Paging and enable 4KB paging
    mov ecx, cr4
    and ecx, 0xffffffef
    mov cr4, ecx
    ; Restore old register values
    popad
    popfd
    ; Load old IDT
    lidt    [idt_descriptor]
    ret

bits 16
bios_call_16_start:
    ; Load data segment
    mov dx,0x20
    mov ds,dx
    mov es,dx
    mov fs,dx
    mov gs,dx
    mov ss,dx

    ; Load real mode IDT
    lidt [0x7000]

    ; Save esp to 0x0ffc (last 4 bytes of memory reserved for the 16-bit code code)
    mov eax,0x5000
    mov [eax],esp

    ; Turn of protected mode via cr0 (also disable write protection)
    mov eax,cr0
    and eax,0x7ffefffe
    mov cr0,eax

    ; Flush pipeline and switch decoding unit to real mode by performing a far jump to the next instruction
    ; 0x0400:0x001b = (0x0400 << 4) + 0x001b = 0x4000 + 0x401b = 0x401b
    jmp 0x0000:(0x4000 + bios_call_16_real_mode_enter - bios_call_16_start)

bios_call_16_real_mode_enter:
    ; Setup segment registers
    mov dx,0x0000
    mov ds,dx
    mov es,dx
    mov fs,dx
    mov gs,dx
    mov ss,dx

    ; Set esp to point directly to the context struct at address 0x6000
    mov esp,0x6000

    ; Enable interrupts
    sti

    ; Pop parameters from the struct (now on stack) into the CPU registers
    pop ds
    pop es
    pop fs
    pop ax ; Only to skip flags
    popad

    ; Perform BIOS call (value is manually placed into memory by Bios::interrupt())
bios_call_16_interrupt:
    int 0x00

    ; Push registers into the parameter struct, which then holds return values from the BIOS call
    pushad
    pushf    ; Do not skip flags this time
    push fs
    push es
    push ds

    ; Disable interrupts
    cli

    ; Enable protected mode (without paging) and write protection
    mov eax,cr0
    or eax,0x00010001
    mov cr0,eax

    ; Flush pipeline and switch decoding unit to protected mode by performing a far jump to the next instruction
    ; 0x0018:0x0049 = GDT[0x0018] + 0x0049 = 0x4000 + 0x0049 = 0x4049
    jmp 0x0018:(0x4000 + (bios_call_16_real_mode_leave - bios_call_16_start))

    ; Restore segment registers
bios_call_16_real_mode_leave:
    mov dx,0x0010
    mov ds,dx
    mov es,dx
    mov fs,dx
    mov gs,dx
    mov ss,dx

    ; Restore esp from 0x5000
    mov eax,0x5000
    mov esp,[eax]

    ; Far return to bios.asm
bits 32
    retfw

bios_call_16_end:

section .data

; Indicates whether the scheduler has been started
scheduler_initialized:
    dw 0