; Copyright (C) 2018 Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schoettner
; Heinrich-Heine University
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
; the kernel is mapped to high addresses with 4mb paging first, thenSp
; a 4kb page-directory is built up

; Created by Burak Akguel, Christian Gesse, Filip Krakowski, Fabian Ruhland, Michael Schoettner, HHU 2018

%include "kernel/constants.asm"

global paging_bootstrap
global enable_4KB_paging
global load_page_directory
global BIOS_Page_Directory

extern on_paging_enabled
extern bootstrapPaging

; calculate index to 4mb page where kernel should be placed
KERNEL_PG_NUM equ (KERNEL_START >> 22)

[SECTION .data]
; all paging stuff has to be 4kb - aligned
align 0x1000
; 4MB - PageTable
; maps first 8MB to 3GB (higher half)
; and maps 8 to 12 MB to 3.5GB (page table/dir area)
legacyPageDirectory:
    dd 0x00000083
    dd 0x00400083
    times (KERNEL_PG_NUM - 2) dd 0
    dd 0x00000083
    dd 0x00400083
    times (893 - 2 - KERNEL_PG_NUM) dd 0
    dd 0x00800083
    times (1024 - 893 - 1) dd 0

Bootstrap_Page_Directory:
    times (1024) dd 0

BIOS_Page_Directory_Legacy:
    dd 0x00000083
    times (KERNEL_PG_NUM - 1) dd 0
    dd 0x00000083
    times (1024 - 1 - KERNEL_PG_NUM) dd 0

; Page directory with 4mb pages for BIOS-calls
BIOS_Page_Directory:
    times (1024) dd 0


[SECTION .text]
; set up first pagetable with 4MB pages to map kernel
; to higher half
paging_bootstrap:

    mov ecx, BIOS_Page_Directory
    sub ecx, KERNEL_START
    push ecx

    mov ecx, Bootstrap_Page_Directory
    sub ecx, KERNEL_START
    push ecx

    call bootstrapPaging
    add esp, 0x08

debug_label:
    ; load address of 4mb - page directory
    mov ecx, Bootstrap_Page_Directory
    ; calculate phys. address since paging is not enabled yet
    sub ecx, KERNEL_START
    ; load cr3 with phys. address of page directory
    mov cr3, ecx

    ; enable pse bit for 4mb paging
    mov ecx, cr4
    or  ecx, 0x00000010
    mov cr4, ecx

    ; enable paging and page protection in cr0
    mov ecx, cr0
    or ecx, 0x80010000
    mov cr0, ecx

    ; jump back to startup sequence in startup.asm
    lea ecx, [on_paging_enabled]
    jmp ecx

; switch from 4mb paging to 4kb paging
enable_4KB_paging:
    mov ecx, cr4
    and ecx, 0xFFFFFFEF
    mov cr4, ecx
    ret


; load phys. address of page directory into cr3
load_page_directory:
	; address is passed as an parameter
    mov	ecx,[4+esp]
    mov cr3, ecx
    ret
