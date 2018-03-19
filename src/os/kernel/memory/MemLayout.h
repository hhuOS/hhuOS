/*****************************************************************************
 *                                                                           *
 *                         M E M L A Y O U T                                 *
 *                                                                           *
 *---------------------------------------------------------------------------*
 * Beschreibung:    Describes the memory layout of hhuOS and defines global  *
 *                  constants                                                *
 *                                                                           *
 * Autor:           Burak Akgül, Christian Gesse, HHU, 2017                  *
 *****************************************************************************/
#ifndef __MEMLAYOUT_include__
#define __MEMLAYOUT_include__

// let kernelspace start at 3GB
#define KERNEL_START 0xC0000000

// start address for heap
#define PHYS_KERNEL_HEAP_START 0x400000
#define VIRT_KERNEL_HEAP_START (PHYS_KERNEL_HEAP_START + KERNEL_START)
// end of virtual kernel memory for heap
#define VIRT_KERNEL_HEAP_END (VIRT_PAGE_MEM_START)
// start of virtual area for page tables and directories (3.5 GB)
#define VIRT_PAGE_MEM_START (KERNEL_START + 0x1F400000)
// end of virtual area for page tables and directories (the 4MB are for kernel stacks)
#define VIRT_PAGE_MEM_END (VIRT_IO_START - 0x40000)
// start of virtual IO space
#define VIRT_IO_START (KERNEL_START + 0x2EE00000)
#define VIRT_IO_END (0x100000000)

// Cap for physical memory - I/O addresses above
#define PHYS_MEM_CAP (0xEA600000)

// begin of kernel code
#define PHYS_SYS_CODE 0x100000
#define VIRT_SYS_CODE (PHYS_SYS_CODE + KERNEL_START)

 // start address of cga memory
 #define PHYS_CGA_START 0xB8000
 #define VIRT_CGA_START (PHYS_CGA_START + KERNEL_START)

 // return address for bios calls
 #define PHYS_BIOS_RETURN_MEM 0x9F000
 #define VIRT_BIOS_RETURN_MEM (PHYS_BIOS_RETURN_MEM + KERNEL_START)

 // Look into startup.asm for corresponding GDT-Entry
 #define PHYS_BIOS16_CODE_MEMORY_START 0x24000 
 #define VIRT_BIOS16_CODE_MEMORY_START (PHYS_BIOS16_CODE_MEMORY_START + KERNEL_START)
 
 // Parameter for BIOS-Calls
 #define PHYS_BIOS16_PARAM_BASE 0x26000
 #define VIRT_BIOS16_PARAM_BASE (PHYS_BIOS16_PARAM_BASE + KERNEL_START)

 // macros for address calculation
 #define VIRT2PHYS(x) ((x) - KERNEL_START)    // same as V2P, but without casts
 #define PHYS2VIRT(x) ((x) + KERNEL_START) // same as P2V, but without casts


 #endif
