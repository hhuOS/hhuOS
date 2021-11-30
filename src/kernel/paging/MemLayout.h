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

#include <cstdint>
#include <lib/util/memory/Address.h>
#include <asm_interface.h>

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

        [[nodiscard]] Util::Memory::Address<uint32_t> toAddress() const {
            return Util::Memory::Address<uint32_t>(startAddress, getSize());
        }

        [[nodiscard]] MemoryArea toPhysical() const {
            if (type == PHYSICAL) {
                return { startAddress, endAddress, PHYSICAL };
            } else {
                return { startAddress - KERNEL_START, endAddress - KERNEL_START, PHYSICAL};
            }
        }

        [[nodiscard]] MemoryArea toVirtual() const {
            if (type == VIRTUAL) {
                return { startAddress, endAddress, VIRTUAL };
            } else {
                return { startAddress + KERNEL_START, endAddress + KERNEL_START, VIRTUAL };
            }
        }
    };

    // Real mode address space (https://wiki.osdev.org/Memory_Map_%28x86%29)
    static const constexpr MemoryArea REAL_MODE_INTERRUPT_VECTOR_TABLE = { 0x00000000, 0x000003ff, MemoryArea::PHYSICAL };
    static const constexpr MemoryArea BIOS_DATA_AREA = { 0x00000400, 0x000004ff, MemoryArea::PHYSICAL };
    static const constexpr MemoryArea USABLE_LOW_MEMORY = { 0x00000500, 0x0007ffff, MemoryArea::PHYSICAL };
    static const constexpr MemoryArea EXTENDED_BIOS_DATA_AREA = { 0x00080000, 0x0009ffff, MemoryArea::PHYSICAL };
    static const constexpr MemoryArea VIDEO_DISPLAY_MEMORY = { 0x000a0000, 0x000bffff, MemoryArea::PHYSICAL };
    static const constexpr MemoryArea VIDEO_BIOS = { 0x000c0000, 0x000c7fff, MemoryArea::PHYSICAL };
    static const constexpr MemoryArea BIOS_EXPANSIONS = { 0x000c8000, 0x000effff, MemoryArea::PHYSICAL };
    static const constexpr MemoryArea MOTHERBOARD_BIOS = { 0x000f0000, 0x000fffff, MemoryArea::PHYSICAL };

    // let kernel start at 3GB
    static const constexpr uint32_t KERNEL_START = 0xc0000000;
    static const constexpr uint32_t MEMORY_END = 0xffffffff;
    
    // start of virtual area for page tables and directories (128 MB)
    static const constexpr MemoryArea PAGING_AREA = {0xf8000000, MEMORY_END, MemoryArea::VIRTUAL };
    // end of virtual kernel memory for heap
    static const constexpr uint32_t KERNEL_HEAP_END_ADDRESS = PAGING_AREA.startAddress - 1;
    
    // return address for bios calls
    static const constexpr MemoryArea VESA_RETURN_MEMORY = { 0x0009f000, 0x0009f200, MemoryArea::PHYSICAL };
    
    // Look into boot.asm for corresponding GDT-Entry
    static const constexpr MemoryArea BIOS_CODE_MEMORY = {0x00004000, 0x00004fff, MemoryArea::PHYSICAL };
    
    // Parameter for BIOS-Calls
    static const constexpr MemoryArea BIOS_PARAMETER_MEMORY = { 0x00006000, 0x00006027, MemoryArea::PHYSICAL };
};

}

#endif
