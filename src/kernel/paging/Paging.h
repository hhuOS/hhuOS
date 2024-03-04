/*
 * Copyright (C) 2018-2024 Heinrich-Heine-Universitaet Duesseldorf,
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
