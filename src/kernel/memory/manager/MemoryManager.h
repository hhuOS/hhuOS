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

#ifndef __KERNEL_MEMORY_MEMORYMANAGER_H__
#define __KERNEL_MEMORY_MEMORYMANAGER_H__

#include <cstdint>
#include "lib/util/memory/String.h"
#include "lib/util/data/HashMap.h"
#include "device/cpu/Cpu.h"

namespace Kernel {

/**
 * Interface for every memory manager.
 *
 * @author Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schoettner
 * @date 2018
 */
class MemoryManager {

public:
    /**
     * Constructor.
     */
    MemoryManager() = default;

    /**
     * Constructor.
     *
     * @param startAddress Start address of the memory area to manage
     * @param endAddress End address of the memory area to manage
     */
    MemoryManager(uint32_t startAddress, uint32_t endAddress);

    /**
     * Copy constructor.
     */
    MemoryManager(const MemoryManager &copy) = delete;

    /**
     * Assignment operator.
     */
    MemoryManager& operator=(const MemoryManager &other) = delete;

    /**
     * Destructor.
     */
    virtual~MemoryManager() = default;

    /**
     * Allocate a chunk of memory of a given size.
     *
     * @param size Amount of memory to allocate
     *
     * @return Pointer to the allocated chunk of memory or nullptr if no chunk with the required size is available
     */
    virtual void *alloc(uint32_t size) = 0;

    /**
     * Free an allocated block of memory.
     *
     * @param ptr Pointer to chunk of memory memory to be freed
     */
    virtual void free(void *ptr) = 0;

    /**
     * Get the start address of the managed memory.
     */
    [[nodiscard]] uint32_t getStartAddress() const;

    /**
     * Get the end address of the managed memory.
     */
    [[nodiscard]] uint32_t getEndAddress() const;

    /**
     * Get the amount of free memory.
     */
    [[nodiscard]] uint32_t getFreeMemory() const;

protected:

    uint32_t memoryStartAddress = 0;
    uint32_t memoryEndAddress = 0;
    uint32_t freeMemory = 0;
};

}

#endif
