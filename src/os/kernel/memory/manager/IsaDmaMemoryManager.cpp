#include "IsaDmaMemoryManager.h"

IsaDmaMemoryManager::IsaDmaMemoryManager() : BitmapMemoryManager(ISA_DMA_START_ADDRESS, ISA_DMA_END_ADDRESS,
        ISA_DMA_BUF_SIZE, true, "ISA_DMA_MEMORY_MANAGER", false) {

    freeBitmapLength = 1;

    freeBitmap = new uint32_t[freeBitmapLength];

    freeBitmap[0] = 0x00ffffff;
}
