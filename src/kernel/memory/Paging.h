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

/* Some macros and constants used for paging
 * 
 * @author Burak Akguel, Christian Gesse, Filip Krakowski, Fabian Ruhland, Michael Schoettner
 * @date 2018
 */

#ifndef __PAGING_H__
#define __PAGING_H__

#include <cstdint>

 //Page Directory only Flags
#define PAGE_SIZE_KiB			        0x000
#define PAGE_SIZE_MiB			        0x080
// this bit will not be used (OS could use it for reserved pages ??)
#define PAGE_SIZE_IGNORED               0x100 

//Page Table only Flags
#define PAGE_NOT_DIRTY		            0x000
#define PAGE_DIRTY				        0x040
#define PAGE_NOT_GLOBAL		            0x000
// prevents the TLB from updating the address in its cache if CR3 is reset (Bit in CR4 must be set)
#define PAGE_GLOBAL				        0x100 

//Common Flags
#define PAGE_NOT_PRESENT 	            0x000
#define PAGE_PRESENT 			        0x001
#define PAGE_READ_ONLY			        0x000
#define PAGE_READ_WRITE			        0x002
#define PAGE_ACCESS_SUPERVISOR	        0x000
#define PAGE_ACCESS_ALL			        0x004
#define PAGE_WRITE_BACK			        0x000
#define PAGE_WRITE_THROUGH		        0x008
#define PAGE_CACHING			        0x000
#define PAGE_NO_CACHING			        0x010
#define PAGE_NOT_ACCESSED		        0x000
#define PAGE_ACCESSED			        0x020
// A protected page will not be unmapped
#define PAGE_WRITE_PROTECTED            0x200

#define GET_PD_IDX(x)                   (x >> 22)
#define GET_PT_IDX(x)                   ((x >> 12) & 0x3FF)
#define GET_OFFSET(x)                   (x & 0xFFF)
#define GET_FLAGS(x)                    (x & 0xFFF)

// pagesize = 4KB
#define PAGESIZE 0x1000

namespace Kernel {

/**
 * Function to set up the 4MB page directories needed for bootstrapping and BIOS-calls.
 * The parameters are assumed to point to physical addresses since paging is not enabled here.
 * In the bootstrap-PD a first initial heap with 4MB and the first 4MB of PagingAreaMemory are mapped
 * because they are needed to bootstrap the final 4KB-paging.
 * Accordingly, until the 4KB paging with pagefault-handling is enabled, the heap should only be used
 * for small allocations so that it does not exceed 4MB.
 *
 * @param directory Pointer to the bootstrapping 4MB page directory
 * @param biosDirectory Pointer to the 4MB page directory only used for BIOS-calls
 */
void bootstrapPaging(uint32_t *directory, uint32_t *biosDirectory);

}

#endif
