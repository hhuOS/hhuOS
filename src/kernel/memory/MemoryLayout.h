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


    // Used for BIOS calls
    static const constexpr MemoryArea BIOS_CALL_CODE_AREA = { 0x00000500, 0x000005ff, MemoryArea::PHYSICAL };
    static const constexpr MemoryArea BIOS_CALL_ESP_BACKUP = { 0x00000600, 0x00000603 + sizeof(uint32_t), MemoryArea::PHYSICAL };
    static const constexpr MemoryArea BIOS_CALL_IDT = { 0x00000604, 0x0000060a + sizeof(uint16_t) + sizeof(uint32_t), MemoryArea::PHYSICAL };
    static const constexpr MemoryArea BIOS_CALL_STACK = { 0x00000700, 0x000007ff, MemoryArea::PHYSICAL };

    // Used to boot up application processors
    static const constexpr MemoryArea APPLICATION_PROCESSOR_STARTUP_CODE = { 0x00001000, 0x00001fff, MemoryArea::PHYSICAL };

    // Used for bootstrapping
    static const constexpr MemoryArea USABLE_LOWER_MEMORY = { 0x00002000, 0x0007ffff, MemoryArea::PHYSICAL };

    // Let kernel start at 1 MiB
    static const constexpr uint32_t KERNEL_START = 0x00100000;

    // Size of virtual memory area for page tables and directories
    static const constexpr uint32_t PAGING_AREA_SIZE = 16 * 1024 * 1024;

    // End of virtual kernel memory
    static const constexpr uint32_t KERNEL_HEAP_END_ADDRESS = 0x8000000;
    static const constexpr uint32_t KERNEL_END = KERNEL_HEAP_END_ADDRESS;

    // The whole virtual kernel area, including code, paging area and heap
    static const constexpr MemoryArea KERNEL_AREA = { KERNEL_START, KERNEL_END, MemoryArea::VIRTUAL };

    // Highest possible address
    static const constexpr uint32_t MEMORY_END = 0xffffffff;
};

}

#endif
