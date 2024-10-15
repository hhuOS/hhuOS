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

#include "GlobalDescriptorTable.h"
#include "lib/util/base/Exception.h"

namespace Kernel {

GlobalDescriptorTable::SegmentDescriptor::SegmentDescriptor(uint32_t base, uint32_t limit, uint8_t access, uint8_t flags) :
        limit1(static_cast<uint16_t>(limit & 0x0000ffff)),
        base1(static_cast<uint16_t>(base & 0x0000ffff)),
        base2(static_cast<uint8_t>((base & 0x00ff0000) >> 16)),
        access(access),
        limit2(static_cast<uint8_t>((limit & 0x000f0000) >> 16)),
        flags(flags),
        base3(static_cast<uint8_t>((base & 0xff000000) >> 24)) {}

GlobalDescriptorTable::Descriptor::Descriptor(const void *address, uint16_t entries) : size((entries * sizeof(uint64_t)) - 1), offset(reinterpret_cast<uint32_t>(address)) {}

GlobalDescriptorTable::SegmentDescriptor::operator uint64_t() const {
    return *reinterpret_cast<const uint64_t*>(this);
}

void GlobalDescriptorTable::addSegment(const GlobalDescriptorTable::SegmentDescriptor &descriptor) {
    if (index >= sizeof(table)) {
        Util::Exception::throwException(Util::Exception::OUT_OF_BOUNDS, "GDT: Table is full!");
    }

    table[index++] = descriptor;
}

GlobalDescriptorTable::Descriptor GlobalDescriptorTable::getDescriptor() const {
    return Descriptor(table, index);
}

void GlobalDescriptorTable::Descriptor::load() {
    asm volatile(
            "lgdt (%0)"
            : :
            "r"(this), "m"(*this)  // input
            :
            );
}

void GlobalDescriptorTable::load() const {
    getDescriptor().load();
}

}