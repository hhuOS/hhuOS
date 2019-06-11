#ifndef HHUOS_ATOMICBITMAPMEMORYMANAGER_H
#define HHUOS_ATOMICBITMAPMEMORYMANAGER_H

#include "lib/async/AtomicBitmap.h"

namespace Kernel {

class AtomicBitmapMemoryManager {

private:

    uint32_t memoryStartAddress = 0;
    uint32_t memoryEndAddress = 0;
    uint32_t blockSize = 0;

    uint32_t freeMemory = 0;

    AtomicBitmap bitmap;

public:

    AtomicBitmapMemoryManager(uint32_t startAddress, uint32_t endAddress, uint32_t blockSize);


    AtomicBitmapMemoryManager(const AtomicBitmapMemoryManager &copy) = delete;


    ~AtomicBitmapMemoryManager() = default;

    void *allocateBlock();

    void freeBlock(void *ptr);
};

}

#endif
