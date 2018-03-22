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

#include "MemoryManager.h"
#include <stdint.h>

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

/**
 * this struct is the return value of an alloc and contains all important
 * information about the allocated memory. It must be stored in each devices
 * class because it is used to free the corresponding memory block
 */
struct IOMemInfo{
    uint32_t virtStartAddress;  // virtual start address of allocated memory
    uint32_t pageCount;         // size of virtual area in pages
    uint32_t* physAddresses;    // Array with physical addresses for the allocated pages
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
    IOMemFreeHeader* anchor = 0;

public:
    /**
     * Constructor
     */
    IOMemoryManager();
    
    // the IO memory manager must never be deleted
    ~IOMemoryManager() = delete;

    /**
     * Initialize memory manager and set up free list.
     */
    void init();

    /**
     * Allocate some virtual 4kb pages for given physical page frames.
     *
     * @param physAddresses Array with physical addresses of the pages that need virtual IO Memory
     * @param pageCnt Amount of pages to be mapped
     * @return IOMemInfo with all information about allocations
     */
    IOMemInfo alloc(uint32_t* physAddresses, uint32_t pageCnt);

    /**
     * Free virtual IO memory.
     *
     * @param memInfo IOMemInfo strcut with all information regarding the memory block
     */
    void free(IOMemInfo memInfo);

    /**
     * Print dump of the free list.
     */
    void dump();
};

#endif
