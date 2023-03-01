/*
 * Copyright (C) 2018-2023 Heinrich-Heine-Universitaet Duesseldorf,
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

#ifndef HHUOS_HEAPMEMORYMANAGER_H
#define HHUOS_HEAPMEMORYMANAGER_H

#include "lib/util/reflection/Prototype.h"
#include "MemoryManager.h"

namespace Util {

class HeapMemoryManager : public MemoryManager, public Util::Reflection::Prototype {

public:
    /**
     * Constructor.
     */
    HeapMemoryManager() = default;

    /**
     * Copy Constructor.
     */
    HeapMemoryManager(const HeapMemoryManager &copy) = delete;

    /**
     * Assignment operator.
     */
    HeapMemoryManager &operator=(const HeapMemoryManager &other) = delete;

    /**
     * Destructor.
     */
    ~HeapMemoryManager() override = default;

    /**
     * Initialize the memory manager.
     *
     * @param startAddress Start address of the memory area to manage
     * @param endAddress End address of the memory area to manage
     */
    virtual void initialize(uint32_t startAddress, uint32_t endAddress) = 0;

    /**
     * Allocate a chunk of memory of a given size and alignment.
     *
     * @param size Amount of memory to allocate
	 * @param alignment Alignment of the allocated chunk
     *
     * @return Pointer to the allocated chunk of memory or nullptr if no chunk with the required size is available
     */
    [[nodiscard]] virtual void* allocateMemory(uint32_t size, uint32_t alignment) = 0;

    /**
     * Reallocate a block of memory of a given size and alignment.
     *
     * If a new chunk needs to be allocated for the reallocation, the content
     * of the old chunk is copied into the new one up to the lesser of the new and old sizes.
     *
     * Reallocation may not be supported by every memory manager.
     *
     * @param pointer Pointer to the chunk of memory to reallocate
     * @param size Amount of new memory to allocate
	 * @param alignment Alignment of the allocated chunk
     *
     * @return Pointer to the reallocated chunk of memory or nullptr if no chunk with the required size is available
     */
    [[nodiscard]] virtual void* reallocateMemory(void *pointer, uint32_t size, uint32_t alignment) = 0;

    /**
     * Free an allocated block of memory.
     *
     * @param pointer Pointer to chunk of memory memory to be freed
	 * @param alignment Alignment of the allocated chunk
     */
    virtual void freeMemory(void *pointer, uint32_t alignment) = 0;
};

}

#endif
