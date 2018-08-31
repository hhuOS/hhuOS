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
#include "MemoryManager.h"

#define DEBUG_IOMEM 0

/**
 * Header for the free list. This linked list is located at the beginning of
 * each free memory block and is used for allocations.
 */
struct IOMemFreeHeader{
    IOMemFreeHeader* prev;      // previous free memory block
    IOMemFreeHeader* next;      // next free memory block
    uint32_t pageCount;         // size of block in 4kb-pages
};


/* IOMemoryManager - Manages IO-Space for HW-Buffers, DMA etc.
*  Maps a given physical address (for example LFB) into the virtual IO-space
*  as defined in MemLayout.h or allocates an 4kb-aligned physical buffer and
*  maps it into virtual IO-space.
*
 * @author Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schoettner
 * @date HHU, 2018
 */
class IOMemoryManager : public MemoryManager {
private:
    // anchor of the free list
    IOMemFreeHeader* anchor = nullptr;

    Util::HashMap<void*, uint32_t> ioMemoryMap;

    Spinlock lock;

public:
    /**
     * Constructor
     */
    IOMemoryManager();
    
    // the IO memory manager must never be deleted
    ~IOMemoryManager() override = default;

    /**
     * Allocate some virtual 4kb pages for given physical page frames.
     *
     * @param size Requested size of memory block
     * @return Start address of memory block
     */
    void * alloc(uint32_t size) override;

    /**
     * Free virtual IO memory.
     *
     * @param ptr Pointer to memory block
     */
    void free(void *ptr) override;

    /**
     * Print dump of the free list.
     */
    void dump();
};

#endif
