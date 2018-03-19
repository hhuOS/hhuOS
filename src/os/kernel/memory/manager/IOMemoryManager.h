/* IOMemoryManager - Manages IO-Space for HW-Buffers, DMA etc.
*  Maps a given physical address (for example LFB) into the virtual IO-space
*  as defined in MemLayout.h or allocates an 4kb-aligned physical buffer and 
*  maps it into virtual IO-space.
*
*  Authors: Burak Akgül and Christian Gesse, HHU 2017  */

#ifndef __IOMEMMANAGER_H__
#define __IOMEMMANAGER_H__

#include "MemoryManager.h"
#include <stdint.h>

#define DEBUG_IOMEM 0

// Header for the free list. This linked list is located at the beginning of 
// each free memory block and is used for allocations.
struct IOMemFreeHeader{
    IOMemFreeHeader* prev;      // previous free memory block
    IOMemFreeHeader* next;      // next free memory block
    uint32_t pageCount;         // size of block in 4kb-pages
};

// this struct is the return value of an alloc and contains all important
// information about the allocated memory. It must be stored in each devices
// class because it is used to free the corresponding memory block
struct IOMemInfo{
    uint32_t virtStartAddress;  // virtual start address of allocated memory
    uint32_t pageCount;         // size of virtual area in pages
    uint32_t* physAddresses;    // Array with physical addresses for the allocated pages
};


class IOMemoryManager : public MemoryManager {
private:
    // anchor of the free list
    IOMemFreeHeader* anchor = 0;

public:
    IOMemoryManager();
    
    // init the IO-space memory manager
    void init();
    // allocate some virtual IO-space and map it to a given physical address
    // should be used if the hardware returns some physical buffer address
    // for example LFB
    IOMemInfo alloc(uint32_t* physAddresses, uint32_t pageCnt);

    // free an allocated IO-memory-block
    void free(IOMemInfo memInfo);

    // print dump of the free list
    void dump();
};

#endif
