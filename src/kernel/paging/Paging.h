/*
 * Copyright (C) 2018-2022 Heinrich-Heine-Universitaet Duesseldorf,
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

    enum Flag : uint32_t {
        NONE = 0x00,

        // System defined flags
        PRESENT = 0x01,
        READ_WRITE = 0x02,
        USER_ACCESS = 0x04,
        WRITE_THROUGH = 0x08,
        CACHE_DISABLE = 0x10,
        ACCESSED = 0x20,
        DIRTY = 0x40,
        PAGE_SIZE_MIB = 0x80,
        GLOBAL = 0x100,

        // User defined flags
        DO_NOT_UNMAP = 0x200
    };

    /**
     * Default Constructor.
     * Deleted, as this class has only static members.
     */
    Paging() = delete;

    /**
     * Copy Constructor.
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
    
    // pagesize = 4KB
    static const constexpr uint32_t PAGESIZE = 0x1000;
    
};

}

#endif
