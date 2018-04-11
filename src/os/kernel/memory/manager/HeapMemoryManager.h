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

#ifndef __HEAPMEMORYMANAGER_H__
#define __HEAPMEMORYMANAGER_H__

#include <stdint.h>
#include "kernel/lock/Spinlock.h"
#include "MemoryManager.h"

/**
 * Header for the double-linked list mamaging the memory blocks.
 */
struct Chunk{
	Chunk* prev;		// previous Chunk
	Chunk* next;		// next Chunk
	bool allocated;		// is this Chunk allocated?
	uint32_t size;		// amount of usable memory in this memory chunk
};

/* List-based memory manager for the kernel heap
 *
 * @author Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schoettner
 * @date HHU, 2018
 */
class HeapMemoryManager : public MemoryManager {

private:
    // Lock for Memory Management
    Spinlock* lock = nullptr;

    // Pointer to first entry in memory list
    Chunk* firstChunk;

public:

    /**
     * Constructor.
     *
     * @param memoryStartAddress Start address of memory area to manage.
     * @param memoryEndAddress  End address of memory area to manage.
     */
    HeapMemoryManager(uint32_t memoryStartAddress, uint32_t memoryEndAddress);

    /**
     * Destructor.
     */
    virtual ~HeapMemoryManager();

    /**
     * Initializes this manager. Sets up first list-entry.
     */
    void init();

    /**
     * Allocate memory block with given size.
     *
     * @param size Size of memory block to be allocated
     * @return Pointer to the allocated memory block
     */
    void* alloc(uint32_t size);

    /**
     * Frees a given memory block
     *
     * @param ptr Pointer to the memory block to be freed
     */
    void free(void* ptr);

    /**
     * Dump memory list
     */
    void dump();
};

#endif
