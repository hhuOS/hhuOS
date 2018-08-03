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

#ifndef __FREELISTMEMORYMANAGER_H__
#define __FREELISTMEMORYMANAGER_H__

#include <cstdint>
#include "lib/lock/Spinlock.h"
#include "MemoryManager.h"

#define THROW_EXCEPTION 0
#define CHECK_MEMORY 0



/* List-based memory manager for the kernel heap
 *
 * @author Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schoettner
 * @date HHU, 2018
 */
class FreeListMemoryManager : public MemoryManager {

private:

    /**
     * Header for the double-linked list mamaging the memory blocks.
     */
    struct FLHeader {
        FLHeader* prev;		// previous Chunk
        FLHeader* next;		// next Chunk
        uint32_t size;		// amount of usable memory in this memory chunk
    };

    // Lock for Memory Management
    Spinlock lock;

    // Pointer to first entry in memory list
    FLHeader* firstChunk;

    FLHeader *findNext(FLHeader *start, uint32_t reqSize);

    FLHeader *merge(FLHeader *origin);

    static const constexpr uint32_t MIN_BLOCK_SIZE = 4;

    static const constexpr uint32_t HEADER_SIZE = sizeof(FLHeader);

public:

    /**
     * Constructor.
     *
     * @param memoryStartAddress Start address of memory area to manage.
     * @param memoryEndAddress  End address of memory area to manage.
     */
    FreeListMemoryManager(uint32_t memoryEndAddress, uint32_t memoryStartAddress, bool doUnmap);

    /**
     * Destructor.
     */
    virtual ~FreeListMemoryManager() = default;

    /**
     * Allocate memory block with given size.
     *
     * @param size Size of memory block to be allocated
     * @return Pointer to the allocated memory block
     */
    void* alloc(uint32_t size) override;

    void* alloc(uint32_t size, uint32_t alignment) override;

    /**
     * Frees a given memory block
     *
     * @param ptr Pointer to the memory block to be freed
     */
    void free(void* ptr) override;

    /**
     * Dump memory list
     */
    void dump();
};

#endif
