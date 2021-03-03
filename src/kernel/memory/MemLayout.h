/*
 * Copyright (C) 2018-2021 Heinrich-Heine-Universitaet Duesseldorf,
 * Institute of Computer Science, Department Operating Systems
 * Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schoettner
 *
 * This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any
 * later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>
 */

/**
 * Describes the memory layout of hhuOS and defines global constants
 *
 * @author Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schoettner
 * @date HHU, 2018
 */

#ifndef __MEMLAYOUT_include__
#define __MEMLAYOUT_include__

// let kernelspace start at 3GB
#define KERNEL_START 0xC0000000

// start address for heap
#define PHYS_KERNEL_HEAP_START 0x400000
#define VIRT_KERNEL_HEAP_START (PHYS_KERNEL_HEAP_START + KERNEL_START)
// start of virtual area for page tables and directories (3.5 GB)
#define VIRT_PAGE_MEM_START (KERNEL_START + 0x1F400000)
// end of virtual kernel memory for heap
#define VIRT_KERNEL_HEAP_END (VIRT_PAGE_MEM_START)
// start of virtual IO space
#define VIRT_IO_START (KERNEL_START + 0x2EE00000)
// end of virtual area for page tables and directories (the 4MB are for kernel stacks)
#define VIRT_PAGE_MEM_END (VIRT_IO_START - 0x40000)
#define VIRT_IO_END 0xFFFFFFFF

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

// Look into boot.asm for corresponding GDT-Entry
#define PHYS_BIOS16_CODE_MEMORY_START 0x4000
#define VIRT_BIOS16_CODE_MEMORY_START (PHYS_BIOS16_CODE_MEMORY_START + KERNEL_START)

// Parameter for BIOS-Calls
#define PHYS_BIOS16_PARAM_BASE 0x6000
#define VIRT_BIOS16_PARAM_BASE (PHYS_BIOS16_PARAM_BASE + KERNEL_START)

#define ISA_DMA_START_ADDRESS 0x00010000
#define ISA_DMA_END_ADDRESS ISA_DMA_START_ADDRESS + 0x00080000

// macros for address calculation
#define VIRT2PHYS(x) ((x) - KERNEL_START)
#define PHYS2VIRT(x) ((x) + KERNEL_START)

#define VIRT2PHYS_VAR(type, x) (*(type*)(((uint8_t*) &x) - KERNEL_START))
#define PHYS2VIRT_VAR(type, x) (*(type*)(((uint8_t*) &x) + KERNEL_START))

#endif
