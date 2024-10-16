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

#include <stdint.h>

namespace Kernel {

class Paging {
    
public:

    static const constexpr uint32_t ENTRIES_PER_TABLE = 1024;

    enum Flags : uint32_t {
        // System defined flags
        NONE = 0x00,
        PRESENT = 0x01,
        WRITABLE = 0x02,
        USER_ACCESSIBLE = 0x04,
        WRITE_THROUGH = 0x08,
        CACHE_DISABLE = 0x10,
        ACCESSED = 0x20,
        DIRTY = 0x40,
        HUGE_PAGE = 0x80,
        GLOBAL = 0x100,
    };

    struct Entry {
        void set(uint32_t address, uint16_t flags);
        void clear();
        [[nodiscard]] uint32_t getAddress() const;
        [[nodiscard]] uint16_t getFlags() const;
        [[nodiscard]] bool isUnused() const;

    private:
        uint32_t flags : 12;
        uint32_t address : 20;
    } __attribute__ ((packed));

    struct Table {
        Entry& operator[] (uint32_t index);
        void clear();
        bool isEmpty();

    private:
        Entry entries[ENTRIES_PER_TABLE]{};
    } __attribute__ ((packed));

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

    static void loadDirectory(const Table &directory);

    static constexpr uint32_t DIRECTORY_INDEX(uint32_t virtualAddress) {
        return virtualAddress >> 22;
    }

    static constexpr uint32_t TABLE_INDEX(uint32_t virtualAddress) {
        return (virtualAddress >> 12) & 0x000003ff;
    }
};

}

#endif
