#include <kernel/memory/MemLayout.h>
#include "IsaDmaMemoryManager.h"

IsaDmaMemoryManager::IsaDmaMemoryManager() : BitmapMemoryManager(ISA_DMA_START_ADDRESS, ISA_DMA_END_ADDRESS,
        false, ISA_DMA_BUF_SIZE, "ISA_DMA_MEMORY_MANAGER", true) {

    // Reserve every block > 512 KB.
    freeBitmap[0] = 0x00ffffff;
}

void *IsaDmaMemoryManager::allocateDmaBuffer() {
    return alloc(ISA_DMA_BUF_SIZE);
}
