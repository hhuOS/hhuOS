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
;
; The APIC implementation is based on a bachelor's thesis, written by Christoph Urlacher.
; The original source code can be found here: https://github.com/ChUrl/hhuOS

; This code is adapted from the OsDev "SMP" page and SerenityOS (and the hhuOS' boot.asm of course):
; https://github.com/SerenityOS/serenity/blob/master/Kernel/Arch/x86_64/Boot/ap_setup.S (visited on 10/02/23).
; This code is relocated by the OS on SMP initialization, so it has to be position independent.
; Because of the relocation, some variables are contained in the .text section, those get initialized
; during runtime.
; The startup is relatively simple (in comparison to the BSP's startup sequence), because we just reuse
; all the stuff already initialized for the BSP (like GDT, IDT, cr0, cr3, cr4).

; Export
global boot_ap
global boot_ap_size
global boot_ap_idtr
global boot_ap_cr0
global boot_ap_cr3
global boot_ap_cr4
global boot_ap_counter ; This is used to identify an APs GDT/Stack and "runningAPs" entry
global boot_ap_gdts
global boot_ap_stacks
global boot_ap_entry

%define startup_address 0x8000
%define stack_size 0x1000

[SECTION .text]
align 8
bits 16
boot_ap:
    ; Disable interrupts (maskable and non-maskable)
    cli
    ; TODO: Does CMOS work core-locally?
    ; mov	al, 0x80
    ; out	0x70, al

    ; Enable A20 address line
    in al, 0x92
    or al, 2
    out 0x92, al

    ; Load the temporary GDT required for the far jump into protected mode.
    ; It is located in the startup memory, so it will be deallocated after AP boot!
    lgdt [tmp_gdt_desc - boot_ap + startup_address]

    ; Enable Protected Mode, must be executed from an identity mapped page (if paging is used).
    ; Our page is identity mapped at startup_address.
    mov eax, cr0
    or al, 0x1 ; Set PE bit
    mov cr0, eax

    ; Setup the protected mode segments
    mov ax, 0x10
    mov ds, ax ; Data segment register
    mov es, ax ; Extra segment register
    mov ss, ax ; Stack segment register
    mov fs, ax ; General purpose segment register
    mov gs, ax ; General purpose segment register

    ; Far jump to protected mode, sets cs (code segment register)
    jmp dword 0x8:boot_ap_32 - boot_ap + startup_address

; Keep these variables in the .text section, so they get copied to startup_address and can be accessed by offsets relative to startup_address.
align 8
tmp_gdt_start:
	dw	0x0, 0x0, 0x0, 0x0
tmp_gdt_code:
	dw	0xFFFF ; 4Gb - (0x100000*0x1000 = 4Gb)
	dw	0x0000 ; base address=0
	dw	0x9A00 ; code read/exec
	dw	0x00CF ; granularity=4096, 386 (+5th nibble of limit)
tmp_gdt_data:
	dw	0xFFFF ; 4Gb - (0x100000*0x1000 = 4Gb)
	dw	0x0000 ; base address=0
	dw	0x9200 ; data read/write
	dw	0x00CF ; granularity=4096, 386 (+5th nibble of limit)
tmp_gdt_end:
tmp_gdt_desc:
	dw tmp_gdt_end - tmp_gdt_start - 1 ; GDT size
	dd tmp_gdt_start - boot_ap + startup_address

; The following is set at runtime by Apic.cpp
align 8
boot_ap_idtr:
    dw 0x0
    dd 0x0
align 8
boot_ap_cr0:
    dd 0x0
align 8
boot_ap_cr3:
    dd 0x0
align 8
boot_ap_cr4:
    dd 0x0
align 8
boot_ap_counter:
    dd 0x0
align 8
boot_ap_gdts:
    dd 0x0
align 8
boot_ap_stacks:
    dd 0x0
align 8
boot_ap_entry:
    dd 0x0

; =================================================================================================

bits 32
align 8
boot_ap_32:
    ; 1. Set cr3 to BSP value (for the page directory)
    mov eax, [boot_ap_cr3 - boot_ap + startup_address]
    mov cr3, eax
    ; 2. Set cr0 to BSP value (to enable paging + page protection)
    mov eax, [boot_ap_cr0 - boot_ap + startup_address]
    mov cr0, eax
    ; 3. Set cr4 to BSP value (for PAE + PSE, if enabled)
    mov eax, [boot_ap_cr4 - boot_ap + startup_address]
    mov cr4, eax

    ; Load the system IDT
    lidt [boot_ap_idtr - boot_ap + startup_address]
    ; lgdt [boot_ap_gdtr - boot_ap + startup_address]

    ; Get the local APIC ID of this AP, to locate GDT and stack
    ; mov eax, 0x1
    ; cpuid
    ; shr ebx, 0x18
    ; mov edi, ebx ; Now the ID is in EDI

    ; Get the initializedApplicationProcessorsCounter value to identify GDT and Stack
    mov eax, [boot_ap_counter - boot_ap + startup_address]
    mov edi, [eax]

    ; Load the AP's prepared GDT and TSS
    mov ebx, [boot_ap_gdts - boot_ap + startup_address] ; GDT descriptor array
    mov eax, [ebx + edi * 0x4] ; Choose correct GDT, each boot_ap_gdts entry is a 4 byte pointer to a descriptor
    lgdt [eax]
    mov ax, 0x28
    ltr ax

    ; Load the correct stack for this AP
    mov ebx, [boot_ap_stacks - boot_ap + startup_address] ; Stackpointer array
    mov esp, [ebx + edi * 0x4] ; Choose correct stack, each boot_ap_stacks entry is a 4 byte pointer to a stack
    add esp, stack_size ; Stack starts at the bottom
    mov ebp, esp ; Update base pointer

    ; Call our entry function
    push edi ; Push apic id
    call [boot_ap_entry - boot_ap + startup_address] ; AP entry function

boot_ap_finish:
    jmp $ ; Should never be reached

[SECTION .data]
align 8
boot_ap_size:
    dw boot_ap_finish - boot_ap + 1 ; Also include the last jmp $
