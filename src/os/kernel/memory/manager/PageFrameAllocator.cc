
#include "PageFrameAllocator.h"

#include "kernel/memory/Paging.h"


PageFrameAllocator::PageFrameAllocator(uint32_t startAddress, uint32_t endAddress)
        : BitmapMemoryManager(startAddress, endAddress, "PAGEFRAMEALLOCATOR", false) {}

void PageFrameAllocator::init() {
    freeMemory = endAddress - startAddress;

    // calculate amount of physical page frames
    uint32_t pageFrameCnt = freeMemory / PAGESIZE;
    // allocate bitmap for page frames
    freeBitMapLength = pageFrameCnt / 32;
    freeBitMap = new uint32_t[freeBitMapLength];

    // first 9 MB are already allocated by 4MB paging -> first 72 Array entries
    for(int i=0; i < 72; i++) {
        freeBitMap[i] = 0xFFFFFFFF;
    }
    // 9 MB + 8KB are already used by kernel and page tables/dirs
    freeBitMap[72] = 0xC0000000;

    initialized = true;
}




