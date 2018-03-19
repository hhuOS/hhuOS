; Contains all functions for paging / virtual memory implemented in assember
; the kernel is mapped to high addresses with 4mb paging first, then
; a 4kb page-directory is built up

; Created by Burak Akguel, Christian Gesse, HHU 2017

[GLOBAL paging_bootstrap]
[GLOBAL KERNEL_START]
[GLOBAL enable_4KB_paging]
[GLOBAL load_page_directory]
[GLOBAL BIOS_Page_Directory]

[EXTERN on_paging_enabled]

; start address of higher half kernel
; TODO: find some way to include this address??
KERNEL_START equ 0xC0000000
; calculate index to 4mb page where kernel should be placed
KERNEL_PG_NUM equ (KERNEL_START >> 22)

[SECTION .data]
; all paging stuff has to be 4kb - aligned
align 0x1000
; 4MB - PageTable
; maps first 8MB to 3GB (higher half)
; and maps 8 to 12 MB to 3.5GB (page table/dir area)
Bootstrap_Page_Directory:
    dd 0x00000083
    dd 0x00400083
    times (KERNEL_PG_NUM - 2) dd 0
    dd 0x00000083
    dd 0x00400083
    times (893 - 2 - KERNEL_PG_NUM) dd 0
    dd 0x00800083
    times (1024 - 893 - 1) dd 0


; Page directory with 4mb pages for BIOS-calls
BIOS_Page_Directory:
    dd 0x00000083
    times (KERNEL_PG_NUM - 1) dd 0
    dd 0x00000083
    times (1024 - 1 - KERNEL_PG_NUM) dd 0


[SECTION .text]
; set up first pagetable with 4MB pages to map kernel
; to higher half
paging_bootstrap:
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

    ; enable paging in cr0
    mov ecx, cr0
    or ecx, 0x80000000
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
    mov	ecx,[4+esp]
    mov cr3, ecx
    ret
