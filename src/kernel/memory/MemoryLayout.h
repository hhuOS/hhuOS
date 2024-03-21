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

/**
 * Describes the memory layout of hhuOS and defines global constants
 *
 * @author Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schoettner
 * @date HHU, 2018
 */

#ifndef __MEMLAYOUT_include__
#define __MEMLAYOUT_include__

#include "lib/util/base/Address.h"


namespace Kernel {

class MemoryLayout {
    
public:

    struct MemoryArea {
        enum Type {
            PHYSICAL,
            VIRTUAL
        };

        const uint32_t startAddress;
        const uint32_t endAddress;
        const Type type;

        [[nodiscard]] uint32_t getSize() const {
            return endAddress - startAddress + 1;
        }

        [[nodiscard]] Util::Address<uint32_t> toAddress() const {
            return Util::Address<uint32_t>(startAddress);
        }
    };

    // let kernel start at 1 MiB
    static const constexpr uint32_t KERNEL_START = 0x00100000;
    static const constexpr uint32_t MEMORY_END = 0xffffffff;

    static const constexpr MemoryArea APPLICATION_PROCESSOR_STARTUP_CODE = { 0x00001000, 0x00001fff, MemoryArea::PHYSICAL };

    static const constexpr MemoryArea USABLE_LOWER_MEMORY = { 0x00002000, 0x0007ffff, MemoryArea::PHYSICAL };

    // start of virtual area for page tables and directories (128 MB)
    static const constexpr uint32_t PAGING_AREA_SIZE = 16 * 1024 * 1024;
    static const constexpr MemoryArea PAGING_AREA = { 0xf8000000, MEMORY_END, MemoryArea::VIRTUAL };
    // end of virtual kernel memory for heap
    static const constexpr uint32_t KERNEL_HEAP_END_ADDRESS = 0x8000000;
    static const constexpr uint32_t KERNEL_END = KERNEL_HEAP_END_ADDRESS;

    static const constexpr MemoryArea KERNEL_AREA = { KERNEL_START, KERNEL_END, MemoryArea::VIRTUAL };
};

}

#endif
