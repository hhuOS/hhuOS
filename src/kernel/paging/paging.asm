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

; Contains all functions for paging / virtual memory implemented in assember
; the kernel is mapped to high addresses with 4MB paging first, thenSp
; a 4KB page-directory is built up

%include "constants.asm"

global enable_system_paging
global load_page_directory
global bios_page_directory

; Calculate index to 4MB page where kernel should be placed
KERNEL_PAGE equ (KERNEL_START >> 22)

section .text

; Switch from 4MB paging to 4KB paging
enable_system_paging:
    mov ecx, cr4
    and ecx, 0xFFFFFFEF
    mov cr4, ecx
    ret

; Load physical address of page directory into cr3
load_page_directory:
    ; The address is passed as a parameter
    mov    ecx,[4+esp]
    mov cr3, ecx
    ret

section .data

; All paging stuff has to be 4KB aligned
align 0x1000

; The following directories are set up in bootstrap_paging

; Page Directory for Bootstrapping
bootstrap_page_directory:
    times (1024) dd 0

; Page directory with 4MB pages for BIOS-calls
bios_page_directory:
    times (1024) dd 0