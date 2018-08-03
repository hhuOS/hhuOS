#include <kernel/memory/MemLayout.h>
#include "IsaDmaMemoryManager.h"

IsaDmaMemoryManager::IsaDmaMemoryManager() : BitmapMemoryManager(ISA_DMA_START_ADDRESS, ISA_DMA_END_ADDRESS,
        ISA_DMA_BUF_SIZE, true, "ISA_DMA_MEMORY_MANAGER", false) {

    // ISA memory space is only 512 KB in size, so the bitmap array has only a single entry.
    freeBitmapLength = 1;
    freeBitmap = new uint32_t[freeBitmapLength];

    // Reserve every block > 512 KB.
    freeBitmap[0] = 0x00ffffff;
}

void *IsaDmaMemoryManager::allocateDmaBuffer() {
    return alloc(ISA_DMA_BUF_SIZE);
}
