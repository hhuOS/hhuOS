#include "AtomicBitmapMemoryManager.h"

AtomicBitmapMemoryManager::AtomicBitmapMemoryManager(uint32_t startAddress, uint32_t endAddress, uint32_t blockSize) :
        memoryStartAddress(startAddress),
        memoryEndAddress(endAddress),
        blockSize(blockSize),
        bitmap((endAddress - startAddress) / blockSize) {

}

void *AtomicBitmapMemoryManager::allocateBlock() {
    uint32_t block = bitmap.findAndSet();

    if(block == bitmap.getSize()) {
        return nullptr;
    }

    freeMemory -= blockSize;

    return reinterpret_cast<void *>(memoryStartAddress + block * blockSize);
}

void AtomicBitmapMemoryManager::freeBlock(void *ptr) {
    uint32_t address = (uint32_t) ptr - memoryStartAddress;

    // check if pointer points to valid memory
    if((uint32_t) ptr < memoryStartAddress || (uint32_t) ptr >= memoryEndAddress) {
        return;
    }

    // find number of block corresponding to physical address
    auto blockNumber = (uint32_t) (address / blockSize);

    bitmap.unset(blockNumber);

    freeMemory += blockSize;
}

