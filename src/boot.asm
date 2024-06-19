; Copyright (C) 2018-2024 Heinrich-Heine-Universitaet Duesseldorf,
; Institute of Computer Science, Department Operating Systems
; Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schoettner
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

; This is the entry-point for the whole system. The switch to protected mode is already done.
; Original by Olaf Spinczyk, TU Dortmund
; Refactored and extended by Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schoettner, HHU

[EXTERN ___BSS_START__]
[EXTERN ___BSS_END__]
[EXTERN ___KERNEL_DATA_START__]
[EXTERN ___KERNEL_DATA_END__]
[EXTERN ___INIT_ARRAY_START__]
[EXTERN ___INIT_ARRAY_END__]
[EXTERN ___FINI_ARRAY_START__]
[EXTERN ___FINI_ARRAY_END__]
[EXTERN main]

[GLOBAL boot]
[GLOBAL _init]
[GLOBAL _fini]
[GLOBAL __cxa_pure_virtual]

; Kernel constants
STACK_SIZE equ 0x10000

; Multiboot2 constants
MULTIBOOT2_HEADER_MAGIC equ 0xe85250d6
MULTIBOOT2_HEADER_ARCHITECTURE equ 0
MULTIBOOT2_HEADER_LENGTH equ (boot - multiboot2_header)
MULTIBOOT2_HEADER_CHECKSUM equ -(MULTIBOOT2_HEADER_MAGIC + MULTIBOOT2_HEADER_ARCHITECTURE + MULTIBOOT2_HEADER_LENGTH)

; Multiboot2 tag types
MULTIBOOT2_TAG_TERMINATE equ 0
MULTIBOOT2_TAG_INFORMATION_REQUEST equ 1
MULTIBOOT2_TAG_ADDRESS equ 2
MULTIBOOT2_TAG_ENTRY_ADDRESS equ 3
MULTIBOOT2_TAG_FLAGS equ 4
MULTIBOOT2_TAG_FRAMEBUFFER equ 5
MULTIBOOT2_TAG_MODULE_ALIGNMENT equ 6
MULTIBOOT2_TAG_EFI_BOOT_SERVICES equ 7
MULTIBOOT2_TAG_EFI_I386_ENTRY_ADDRESS equ 8
MULTIBOOT2_TAG_EFI_AMD64_ENTRY_ADDRESS equ 9
MULTIBOOT2_TAG_RELOCATABLE_HEADER equ 10

; Multiboot2 request types
MULTIBOOT2_REQUEST_BOOT_COMMAND_LINE equ 1
MULTIBOOT2_REQUEST_BOOT_LOADER_NAME equ 2
MULTIBOOT2_REQUEST_MODULE equ 3
MULTIBOOT2_REQUEST_BASIC_MEMORY_INFORMATION equ 4
MULTIBOOT2_REQUEST_BIOS_BOOT_DEVICE equ 5
MULTIBOOT2_REQUEST_MEMORY_MAP equ 6
MULTIBOOT2_REQUEST_VBE_INFO equ 7
MULTIBOOT2_REQUEST_FRAMEBUFFER_INFO equ 8
MULTIBOOT2_REQUEST_ELF_SYMBOLS equ 9
MULTIBOOT2_REQUEST_APM_TABLE equ 10
MULTIBOOT2_REQUEST_EFI_32_BIT_SYSTEM_TABLE_POINTER equ 11
MULTIBOOT2_REQUEST_EFI_64_BIT_SYSTEM_TABLE_POINTER equ 12
MULTIBOOT2_REQUEST_SMBIOS_TABLES equ 13
MULTIBOOT2_REQUEST_ACPI_OLD_RSDP equ 14
MULTIBOOT2_REQUEST_ACPI_NEW_RSDP equ 15
MULTIBOOT2_REQUEST_NETWORKING_INFORMATION equ 16
MULTIBOOT2_REQUEST_EFI_MEMORY_MAP equ 17
MULTIBOOT2_REQUEST_EFI_BOOT_SERVICES_NOT_TERMINATED equ 18
MULTIBOOT2_REQUEST_EFI_32_BIT_IMAGE_HANDLE_POINTER equ 19
MULTIBOOT2_REQUEST_EFI_64_BIT_IMAGE_HANDLE_POINTER equ 20
MULTIBOOT2_REQUEST_IMAGE_LOAD_BASE_PHYSICAL_ADDRESS equ 21

; Multiboot2 tag flags
MULTIBOOT2_TAG_FLAG_REQUIRED equ 0x00
MULTIBOOT2_TAG_FLAG_OPTIONAL equ 0x01

; Multiboot2 console flags
MULTIBOOT2_CONSOLE_FLAG_FORCE_TEXT_MODE equ 0x01
MULTIBOOT2_CONSOLE_FLAG_SUPPORT_TEXT_MODE equ 0x02

[SECTION .text]
[BITS 32]

multiboot2_header:
    ; Header
    align 8
    dd MULTIBOOT2_HEADER_MAGIC
    dd MULTIBOOT2_HEADER_ARCHITECTURE
    dd MULTIBOOT2_HEADER_LENGTH
    dd MULTIBOOT2_HEADER_CHECKSUM

    ; Flags tag
    align 8
    dw MULTIBOOT2_TAG_FLAGS
    dw MULTIBOOT2_TAG_FLAG_OPTIONAL
    dd 12
    dd MULTIBOOT2_CONSOLE_FLAG_SUPPORT_TEXT_MODE

    ; Information request tag (required)
    align 8
    dw MULTIBOOT2_TAG_INFORMATION_REQUEST
    dw 0
    dd 24
    dd MULTIBOOT2_REQUEST_BOOT_COMMAND_LINE
    dd MULTIBOOT2_REQUEST_MODULE
    dd MULTIBOOT2_REQUEST_MEMORY_MAP
    dd MULTIBOOT2_REQUEST_FRAMEBUFFER_INFO

    ; Information request tag (optional)
    align 8
    dw MULTIBOOT2_TAG_INFORMATION_REQUEST
    dw MULTIBOOT2_TAG_FLAG_OPTIONAL
    dd 28
    dd MULTIBOOT2_REQUEST_BOOT_LOADER_NAME
    dd MULTIBOOT2_REQUEST_SMBIOS_TABLES
    dd MULTIBOOT2_REQUEST_ACPI_OLD_RSDP
    dd MULTIBOOT2_REQUEST_ACPI_NEW_RSDP
    dd MULTIBOOT2_REQUEST_ELF_SYMBOLS

    ; Framebuffer tag (Delete to let GRUB boot in CGA text mode)
    align 8
    dw MULTIBOOT2_TAG_FRAMEBUFFER
    dw MULTIBOOT2_TAG_FLAG_OPTIONAL
    dd 20
    dd 800
    dd 600
    dd 32

    ; Module alignment tag
    align 8
    dw MULTIBOOT2_TAG_MODULE_ALIGNMENT
    dw MULTIBOOT2_TAG_FLAG_OPTIONAL
    dd 8

    ; Termination tag
    align 8
    dw MULTIBOOT2_TAG_TERMINATE
    dw 0
    dd 8

boot:
    cld ; Expected by GCC
    cli ; Disable interrupts

; Clear BSS sections
    mov edi,___BSS_START__
clear_bss_loop:
    mov byte [edi],0
    inc edi
    cmp edi, ___BSS_END__
    jne clear_bss_loop

    ; Switch to initial kernel stack
    mov esp, init_stack.end

    ; Call C function with multiboot2 magic number and address (located in eax and ebx)
    push ebx
    push eax
    call main

; Call constructors of global objects
_init:
    mov edi, ___INIT_ARRAY_START__
_init_loop:
    cmp edi, ___INIT_ARRAY_END__
    jge _init_done
    call [edi]
    add edi, 4
    jmp _init_loop
_init_done:
    ret

; Call destructors of global objects
_fini:
    mov edi, ___FINI_ARRAY_START__
_fini_loop:
    cmp edi, ___FINI_ARRAY_END__
    jge _fini_done
    call [edi]
    add edi, 4
    jmp _fini_loop
_fini_done:
    ret

; This function is used when global constructors are called
; The label must be defined but can be void
__cxa_pure_virtual:
    ret

; Reserve space for initial kernel stack
[SECTION .bss]

global init_stack:data (init_stack.end - init_stack)
init_stack:
	  resb STACK_SIZE
.end:
