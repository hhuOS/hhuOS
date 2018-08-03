/**
 * BitmapMemoryManager - manages a given area of memory in 4kb blocks using
 * a bitmap mechanism.
 *
 * @author Burak Akguel, Christian Gesse, Filip Krakowski, Fabian Ruhland, Michael Schoettner
 * @date 2018
 */

#include <kernel/Kernel.h>
#include <kernel/Bios.h>
#include "BitmapMemoryManager.h"
#include "kernel/memory/Paging.h"
#include "lib/libc/printf.h"

extern "C" {
#include "lib/libc/string.h"
}

/**
 * Constructor
 */
BitmapMemoryManager::BitmapMemoryManager(uint32_t memoryEndAddress, uint32_t blockSize, String name,
                                         bool zeroMemory, uint32_t memoryStartAddress, bool doUnmap) : MemoryManager(
        memoryStartAddress, memoryEndAddress, doUnmap) {
    this->blockSize = blockSize;
    this->name = name;
    this->bmpSearchOffset = 0;
    this->zeroMemory = zeroMemory;
}

/**
 * Allocate a block of memory
 */
void* BitmapMemoryManager::alloc(uint32_t size) {
    uint32_t blockCount = (size / blockSize) + ((size % blockSize == 0) ? 0 : 1);

    uint32_t arrayIndexStart = 0;
    uint32_t bitmapIndexStart = 0;
    uint32_t freeCount = 0;

    for(uint32_t i = bmpSearchOffset; i < freeBitmapLength; i++) {
        uint32_t currentEntry = freeBitmap[i];

        for(uint8_t j = 32; j > 0; j--) {

            if((currentEntry & 0x80000000) == 0) {
                if(freeCount == 0) {
                    arrayIndexStart = i;
                    bitmapIndexStart = static_cast<uint32_t>(j - 1);
                }

                freeCount++;

                if(freeCount == blockCount) {
                    i = freeBitmapLength;

                    break;
                }
            } else {
                freeCount = 0;
            }

            currentEntry <<= 1U;
        }
    }

    if(freeCount == blockCount) {
        uint32_t i = arrayIndexStart;
        uint32_t j = bitmapIndexStart;

        while(freeCount > 0) {

            freeBitmap[i] |= 1 << j;

            if(j == 0) {
                i++;
                j = 31;
            } else {
                j--;
            }

            freeCount--;
        }

        freeMemory -= blockCount * blockSize;

        if(zeroMemory){
            memset((void*) (memoryStartAddress + (32 * arrayIndexStart + (31 - bitmapIndexStart)) * blockSize), 0, blockCount * blockSize);
        }

        return (void *) (memoryStartAddress + (32 * arrayIndexStart + (31 - bitmapIndexStart)) * blockSize);
    }


    // found no pageframe
    if(managerType == PAGING_AREA_MANAGER) {
        Cpu::throwException(Cpu::Exception::OUT_OF_PAGE_MEMORY);
    }

    if(managerType == PAGE_FRAME_ALLOCATOR) {
        Cpu::throwException(Cpu::Exception::OUT_OF_PHYS_MEMORY);
    }

    if(managerType == MISC) {
        Cpu::throwException(Cpu::Exception::OUT_OF_MEMORY);
    }

    return 0;
}

/**
 * Free a memory block
 */
void BitmapMemoryManager::free(void *ptr) {
    uint32_t address = (uint32_t) ptr - memoryStartAddress;

// check if kernel or i/o space is affected
    if(address < memoryStartAddress || address >= memoryEndAddress) {
#if DEBUG_BMM
        printf("[%s] ERROR: Something went wrong freeing the page - %x\n", name, address);
#endif
        return;
    }

    // find number of block corresponding to physical address
    uint32_t blockNumber = (uint32_t) (address / blockSize);
    // calculate array idx to freeBitMap
    uint16_t arrayIdx = (uint16_t) (blockNumber / 32);
    // calculate shift index
    uint8_t  idx = (uint8_t) (31 - (blockNumber % 32));

    // set bit representing this page to 0
    freeBitmap[arrayIdx] &= ~(1 << idx);
    freeMemory += blockSize;

#if DEBUG_BMM
    printf("[%s] Free block at address %x", name, address);
    printf(" Bitmap Index %d , Value_Offset %d\n", arrayIdx, (uint32_t)idx);
#endif
}

/**
 * Dump bitmap for debugging reasons
 */
void BitmapMemoryManager::dump() {
    printf("Bitmap of memory manager %s\n", name);

    for(uint16_t idx = 0; idx <= freeBitmapLength; idx++) {
        if(idx % 4 == 0){
            printf("\n");
        }
        printf("%x", freeBitmap[idx]);


    }
}
