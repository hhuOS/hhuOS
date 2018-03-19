/* Allocates and frees the page frames in physical memory. 
 * Bitmap-based implementation.
 * Burak Akguel, Christian Gesse, HHU 2017*/

#ifndef __PAGEFRAMEALLOCATOR_H__
#define __PAGEFRAMEALLOCATOR_H__

#include <stdint.h>
#include "kernel/memory/manager/BitmapMemoryManager.h"


class PageFrameAllocator : public BitmapMemoryManager{

public:
    PageFrameAllocator(uint32_t startAddress, uint32_t endAddress);

    // init -> needed instead of constructor, because we want to decide when it
    // time to start paging
    void init();

};

#endif
