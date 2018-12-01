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

#ifndef __IOMEMMANAGER_H__
#define __IOMEMMANAGER_H__

#include <cstdint>
#include "lib/util/HashMap.h"
#include "lib/lock/Spinlock.h"
#include "MemoryManager.h"

/**
 * Header for the free list. This linked list is located at the beginning of
 * each free memory block and is used for allocations.
 */
struct IOMemFreeHeader{
    IOMemFreeHeader* prev;      // previous free memory block
    IOMemFreeHeader* next;      // next free memory block
    uint32_t pageCount;         // size of block in 4kb-pages
};


/**
 * Memory manager, that manages IO-Space for HW-Buffers, DMA etc.
 * Maps a given physical address (for example LFB) into the virtual IO-space
 * as defined in MemLayout.h or allocates an 4kb-aligned physical buffer and
 * maps it into virtual IO-space.
 *
 * It uses a doubly-linked list of free chunks of memory, similarly to FreeListMemoryManager.
 *
 * This memory manager does not allow allocation with an alignment.
 *
 * TODO: Implement realloc.
 *
 * @author Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schoettner
 * @date 2018
 */
class IOMemoryManager : public MemoryManager {

private:

    IOMemFreeHeader* anchor = nullptr;

    Util::HashMap<void*, uint32_t> ioMemoryMap;

    Spinlock lock;

public:

    /**
     * Constructor.
     */
    IOMemoryManager();

    /**
     * Copy-constructor.
     */
    IOMemoryManager(const MemoryManager &copy) = delete;

    /**
     * Destructor.
     */
    ~IOMemoryManager() override = default;

    /**
     * Overriding function from MemoryManager.
     */
    void * alloc(uint32_t size) override;

    /**
     * Overriding function from MemoryManager.
     */
    void free(void *ptr) override;

    /**
     * Dump the list of free chunks of memory.
     */
    void dump() override;
};

#endif
