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

#ifndef HHUOS_GLOBALDESCRIPTORTABLE_H
#define HHUOS_GLOBALDESCRIPTORTABLE_H

#include <cstdint>

namespace Device {

class GlobalDescriptorTable {

public:

    struct SegmentDescriptor {

    public:

        SegmentDescriptor(uint32_t base, uint32_t limit, uint8_t access, uint8_t flags);

        operator uint64_t() const;

    private:
        uint16_t limit1;
        uint16_t base1;
        uint8_t base2;
        uint8_t access;
        uint8_t limit2: 4;
        uint8_t flags: 4;
        uint8_t base3;
    } __attribute__((packed));

    enum Access : uint8_t {
        ACCESSED = 1 << 0,
        WRITABLE = 1 << 1,
        DIRECTION = 1 << 2,
        EXECUTABLE = 1 << 3,
        USER_SEGMENT = 1 << 4,
        PRESENT = 1 << 7
    };

    enum Flags : uint8_t {
        LONG_MODE = 1 << 1,
        BITS_32 = 1 << 2,
        GRANULARITY = 1 << 3
    };

    struct TaskStateSegment {
        uint32_t previousTss;
        uint32_t esp0;      // Points to kernel stack
        uint32_t ss0;       // Points to segment used by kernel stack
        uint32_t esp1;
        uint32_t ss1;
        uint32_t esp2;
        uint32_t ss2;
        uint32_t cr3;
        uint32_t eip;
        uint32_t eflags;
        uint32_t eax;
        uint32_t ecx;
        uint32_t edx;
        uint32_t ebx;
        uint32_t esp;
        uint32_t ebp;
        uint32_t esi;
        uint32_t edi;
        uint32_t es;
        uint32_t cs;
        uint32_t ss;
        uint32_t ds;
        uint32_t fs;
        uint32_t gs;
        uint32_t ldt;
        uint16_t trap;
        uint16_t ioMapBaseOffset = sizeof(TaskStateSegment);
    } __attribute__((packed));

    /**
     * Default Constructor.
     */
    GlobalDescriptorTable() = default;

    /**
     * Copy Constructor.
     */
    GlobalDescriptorTable(const GlobalDescriptorTable &other) = delete;

    /**
     * Assignment operator.
     */
    GlobalDescriptorTable &operator=(const GlobalDescriptorTable &other) = delete;

    /**
     * Destructor.
     */
    ~GlobalDescriptorTable() = default;

    void addSegment(const SegmentDescriptor &descriptor);

    void load();

private:

    struct Descriptor {
        uint16_t size;
        uint32_t offset;
    } __attribute__((packed));

    uint64_t table[8]{};
    uint32_t index = 1; // First entry is always the NULL-descriptor

};

}

#endif
