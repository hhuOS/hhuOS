/*
 * Copyright (C) 2018 Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schoettner
 * Heinrich-Heine University
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
#include "lib/libc/printf.h"
#include "kernel/cpu/Cpu.h"

extern "C" {
#include "lib/libc/string.h"
};

/**
 * Interface for every memory manager.
 *
 * @author Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schoettner
 * @date 2018
 */
class MemoryManager {

protected:
    uint32_t memoryStartAddress;
    uint32_t memoryEndAddress;

    uint32_t freeMemory = 0;

    bool doUnmap;

public:

    /**
     * Constructor.
     *
     * @param memoryStartAddress Start address of the memory area to manage
     * @param memoryEndAddress End address of the memory area to manage
     * @param doUnmap Indicates, whether or not the manager should unmap freed memory by itself
     */
    MemoryManager(uint32_t memoryStartAddress, uint32_t memoryEndAddress, bool doUnmap) {
        this->memoryStartAddress = memoryStartAddress;
        this->memoryEndAddress = memoryEndAddress;
        this->freeMemory = memoryEndAddress - memoryStartAddress;
        this->doUnmap = doUnmap;
    }

    /**
     * Copy-constructor.
     */
    MemoryManager(const MemoryManager &copy) = delete;

    /**
     * Destructor.
     */
    virtual ~MemoryManager() = default;

    /**
     * Allocate a chunk of memory of a given size.
     *
     * @param size Amount of memory to allocate
     *
     * @return Pointer to the allocated chunk of memory or nullptr if no chunk with the required size is available
     */
    virtual void *alloc(uint32_t size) { return nullptr; };

    /**
	 * Allocate an aligned chunk of memory of a given size.
     *
     * This type of allocation may not be supported by every memory manager.
     * For example, it does not make sense to request allocated memory from a bitmap-based manager,
     * as such a manager always returns chunks with the same alignment,
	 *
	 * @param size Amount of memory to allocate
	 * @param alignment Alignment of the allocated chunk
     *
	 * @return Pointer to the allocated chunk of memory or nullptr if no chunk with the required size is available
	 */
    virtual void *alloc(uint32_t size, uint32_t alignment) {
        Cpu::throwException(Cpu::Exception::UNSUPPORTED_OPERATION);

        return nullptr;
    };

    /**
     * Reallocate a block of memory of a given size.
     *
     * If a new chunk needs to be allocated for the reallocation, the content
     * of the old chunk is copied into the new one up to the lesser of the new and old sizes.
     *
     * Reallocation may not be supported by every memory manager.
     *
     * @param ptr Pointer to the chunk of memory to reallocate
     * @param size Amount of new memory to allocate
     *
     * @return Pointer to the reallocated chunk of memory or nullptr if no chunk with the required size is available
     */
    virtual void *realloc(void *ptr, uint32_t size) {
        Cpu::throwException(Cpu::Exception::UNSUPPORTED_OPERATION);

        return nullptr;
    };

    /**
     * Reallocate a block of memory of a given size. The reallocated block will be aligned to a given alignment.
     *
     * If a new chunk needs to be allocated for the reallocation, the content
     * of the old chunk is copied into the new one up to the lesser of the new and old sizes.
     *
     * @param ptr Pointer to the chunk of memory to reallocate
     * @param size Amount of new memory to allocate
	 * @param alignment Alignment of the allocated chunk
     *
     * @return Pointer to the reallocated chunk of memory or nullptr if no chunk with the required size is available
     */
    virtual void *realloc(void *ptr, uint32_t size, uint32_t alignment) {
        Cpu::throwException(Cpu::Exception::UNSUPPORTED_OPERATION);

        return nullptr;
    };

    /**
     * Free an allocated block of memory.
     *
     * @param ptr Pointer to chunk of memory memory to be freed
     */
    virtual void free(void *ptr) {};

    /**
	 * Free an allocated block of memory, that has been allocated with an alignment.
	 *
     * @param ptr Pointer to chunk of memory memory to be freed
	 * @param alignment Alignment of the chunk
	 */
    virtual void free(void *ptr, uint32_t alignment) { Cpu::throwException(Cpu::Exception::UNSUPPORTED_OPERATION); };

    /**
     * Dump the data structure, that a specific implementation uses to keep track of free/allocated memory.
     */
    virtual void dump() {
        printf("MemoryManager: dump() not implemented!\n");
    }

    /**
     * Get the start address of the managed memory.
     */
    uint32_t getStartAddress() {
        return memoryStartAddress;
    }

    /**
     * Get the end address of the managed memory.
     */
    uint32_t getEndAddress() {
        return memoryEndAddress;
    }

   /**
    * Get the amount of free memory.
    */
    uint32_t getFreeMemory() {
        return freeMemory;
    }
};

#endif
