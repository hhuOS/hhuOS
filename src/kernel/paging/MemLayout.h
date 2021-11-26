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

namespace Kernel {

class MemoryLayout {
    
public:

    // let kernel start at 3GB
    static const constexpr uint32_t VIRT_KERNEL_START = 0xC0000000;
    static const constexpr uint32_t VIRT_MEM_END = 0xFFFFFFFF;
    
    // start of virtual area for page tables and directories (128 MB)
    static const constexpr uint32_t VIRT_PAGE_MEM_START = 0xF8000000;
    // end of virtual kernel memory for heap
    static const constexpr uint32_t VIRT_KERNEL_HEAP_END = VIRT_PAGE_MEM_START;
    // end of virtual area for page tables and directories
    static const constexpr uint32_t VIRT_PAGE_MEM_END = VIRT_MEM_END;
    
    // begin of kernel code
    static const constexpr uint32_t PHYS_SYS_CODE = 0x100000;
    static const constexpr uint32_t VIRT_SYS_CODE = PHYS_SYS_CODE + VIRT_KERNEL_START;
    
    // start address of cga memory
    static const constexpr uint32_t PHYS_CGA_START = 0xB8000;
    static const constexpr uint32_t VIRT_CGA_START = PHYS_CGA_START + VIRT_KERNEL_START;
    
    // return address for bios calls
    static const constexpr uint32_t PHYS_BIOS_RETURN_MEM = 0x9F000;
    static const constexpr uint32_t VIRT_BIOS_RETURN_MEM = PHYS_BIOS_RETURN_MEM + VIRT_KERNEL_START;
    
    // Look into boot.asm for corresponding GDT-Entry
    static const constexpr uint32_t PHYS_BIOS16_CODE_MEMORY_START = 0x4000;
    static const constexpr uint32_t VIRT_BIOS16_CODE_MEMORY_START = PHYS_BIOS16_CODE_MEMORY_START + VIRT_KERNEL_START;
    
    // Parameter for BIOS-Calls
    static const constexpr uint32_t PHYS_BIOS16_PARAM_BASE = 0x6000;
    static const constexpr uint32_t VIRT_BIOS16_PARAM_BASE = PHYS_BIOS16_PARAM_BASE + VIRT_KERNEL_START;
    
    static const constexpr uint32_t ISA_DMA_START_ADDRESS = 0x00010000;
    static const constexpr uint32_t ISA_DMA_END_ADDRESS = ISA_DMA_START_ADDRESS + 0x00080000;

    static const constexpr uint32_t VIRT2PHYS(uint32_t address) {
        return address - VIRT_KERNEL_START;
    }

    static const constexpr uint32_t PHYS2VIRT(uint32_t address) {
        return address + VIRT_KERNEL_START;
    }
    
};

}

#endif
