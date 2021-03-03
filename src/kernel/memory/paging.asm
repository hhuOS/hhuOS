; Copyright (C) 2018-2021 Heinrich-Heine-Universitaet Duesseldorf,
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

global enable_bootstrap_paging
global enable_system_paging
global load_page_directory
global bios_page_directory

extern on_paging_enabled
extern bootstrap_paging

; Calculate index to 4MB page where kernel should be placed
KERNEL_PAGE equ (KERNEL_START >> 22)

section .text
; Set up first page table with 4MB pages to map kernel to higher half
enable_bootstrap_paging:
    push bios_page_directory - KERNEL_START
    push bootstrap_page_directory - KERNEL_START
    call bootstrap_paging

    ; Load physical address of 4MB page directory into cr3
    mov ecx, bootstrap_page_directory - KERNEL_START
    mov cr3, ecx

    ; Enable pse bit for 4MB paging
    mov ecx, cr4
    or  ecx, 0x00000010
    mov cr4, ecx

    ; Enable paging and page protection in cr0
    mov ecx, cr0
    or  ecx, 0x80010000
    mov cr0, ecx

    ; Jump back to boot sequence in boot.asm
    lea ecx, [on_paging_enabled]
    jmp ecx

; Switch from 4MB paging to 4KB paging
enable_system_paging:
    mov ecx, cr4
    and ecx, 0xFFFFFFEF
    mov cr4, ecx
    ret

; Load physical address of page directory into cr3
load_page_directory:
	; The address is passed as a parameter
    mov	ecx,[4+esp]
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