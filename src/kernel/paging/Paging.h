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

namespace Kernel {

class Paging {
    
public:
    /**
     * Default Constructor.
     * Deleted, as this class has only static members.
     */
    Paging() = delete;

    /**
     * Copy constructor.
     */
    Paging(const Paging &other) = delete;

    /**
     * Assignment operator.
     */
    Paging &operator=(const Paging &other) = delete;

    /**
     * Destructor.
     */
    ~Paging() = default;

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
    static void bootstrapPaging(uint32_t *directory, uint32_t *biosDirectory);

    static constexpr uint32_t GET_PD_IDX(uint32_t x) {
        return x >> 22;
    }

    static constexpr uint32_t GET_PT_IDX(uint32_t x) {
        return (x >> 12) & 0x3FF;
    }

    static constexpr uint32_t GET_OFFSET(uint32_t x) {
        return x & 0xFFF;
    }

    static constexpr uint32_t GET_FLAGS(uint32_t x) {
        return 0xFFF;
    }

    //Page Directory only Flags
    static const constexpr uint32_t PAGE_SIZE_KiB = 0x000;
    static const constexpr uint32_t PAGE_SIZE_MiB = 0x080;
    // this bit will not be used (OS could use it for reserved pages ??)
    static const constexpr uint32_t PAGE_SIZE_IGNORED = 0x100;
    
    //Page Table only Flags
    static const constexpr uint32_t PAGE_NOT_DIRTY = 0x000;
    static const constexpr uint32_t PAGE_DIRTY = 0x040;
    static const constexpr uint32_t PAGE_NOT_GLOBAL = 0x000;
    // prevents the TLB from updating the address in its cache if CR3 is reset (Bit in CR4 must be set)
    static const constexpr uint32_t PAGE_GLOBAL = 0x100;
    
    //Common Flags
    static const constexpr uint32_t PAGE_NOT_PRESENT = 0x000;
    static const constexpr uint32_t PAGE_PRESENT = 0x001;
    static const constexpr uint32_t PAGE_READ_ONLY = 0x000;
    static const constexpr uint32_t PAGE_READ_WRITE = 0x002;
    static const constexpr uint32_t PAGE_ACCESS_SUPERVISOR = 0x000;
    static const constexpr uint32_t PAGE_ACCESS_ALL	= 0x004;
    static const constexpr uint32_t PAGE_WRITE_BACK	= 0x000;
    static const constexpr uint32_t PAGE_WRITE_THROUGH = 0x008;
    static const constexpr uint32_t PAGE_CACHING = 0x000;
    static const constexpr uint32_t PAGE_NO_CACHING = 0x010;
    static const constexpr uint32_t PAGE_NOT_ACCESSED = 0x000;
    static const constexpr uint32_t PAGE_ACCESSED = 0x020;
    // A protected page will not be unmapped
    static const constexpr uint32_t PAGE_WRITE_PROTECTED = 0x200;
    
    // pagesize = 4KB
    static const constexpr uint32_t PAGESIZE = 0x1000;
    
};

}

#endif
