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

%ifndef CONSTANTS_ASM
%define CONSTANTS_ASM

; Kernel constants
KERNEL_START equ 0xc0000000
STACK_SIZE equ 0x00001000
MULTIBOOT_SIZE equ 0x00010000
ACPI_SIZE equ 0x00010000
SMBIOS_SIZE equ 0x00010000

; BIOS call constants
BIOS_CALL_CODE equ 0x00000500
BIOS_CALL_ESP_BACKUP equ 0x00000600
BIOS_CALL_IDT equ 0x00000604
BIOS_CALL_STACK equ 0x00000700

%endif