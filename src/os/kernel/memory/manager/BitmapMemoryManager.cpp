/**
 * BitmapMemoryManager - manages a given area of memory in blocks of given size using
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
BitmapMemoryManager::BitmapMemoryManager(uint32_t memoryStartAddress, uint32_t memoryEndAddress, uint32_t blockSize,
                                         String name, bool zeroMemory, bool doUnmap) : MemoryManager(
        memoryStartAddress, memoryEndAddress, doUnmap) {
    this->blockSize = blockSize;
    this->name = name;
    this->bmpSearchOffset = 0;
    this->zeroMemory = zeroMemory;
}

BitmapMemoryManager::~BitmapMemoryManager() {
	delete freeBitmap;
}

/**
 * Allocate one or several blocks of memory
 */
void* BitmapMemoryManager::alloc(uint32_t size) {
	// get count of blocks that corresponds to aligned size
    uint32_t blockCount = (size / blockSize) + ((size % blockSize == 0) ? 0 : 1);

    // set up indices of searching
    uint32_t arrayIndexStart = 0;
    uint32_t bitmapIndexStart = 0;
    uint32_t freeCount = 0;

    // search a sequence of free memory blocks
    for(uint32_t i = bmpSearchOffset; i < freeBitmapLength; i++) {
    	// get current bitmap entry
        uint32_t currentEntry = freeBitmap[i];

        // First check if there are free blocks in this entry before starting
        // to search them
        if(currentEntry != 0xFFFFFFFF) {
            // check every block in current bitmap entry
            for(uint8_t j = 32; j > 0; j--) {
            	// check if current block is free
                if((currentEntry & 0x80000000) == 0) {
                	// if yes and we have not marked some blocks earlier, start search here
                    if(freeCount == 0) {
                        arrayIndexStart = i;
                        bitmapIndexStart = static_cast<uint32_t>(j - 1);
                    }
                    // increase free count because we have found a free block
                    freeCount++;

                    // if we have enough free contiguous blocks break here
                    if(freeCount == blockCount) {
                        i = freeBitmapLength;

                        break;
                    }
                // if current block is not free
                // reset free count and continue searching
                } else {
                    freeCount = 0;
                }
                // shift current bitmap entry to examine next block
                currentEntry <<= 1U;
            }
        // if there is no free block in this bitmap entry, set free count to 0
        } else {
        	freeCount = 0;
        }

    }

    // if we have enough free contiguous blocks, we cann mark them as alloced in this loop
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

        // update free memory
        freeMemory -= blockCount * blockSize;

        // zero allocated memory if required
        if(zeroMemory){
            memset((void*) (memoryStartAddress + (32 * arrayIndexStart + (31 - bitmapIndexStart)) * blockSize), 0, blockCount * blockSize);
        }

        // return start address of allocated memory
        return (void *) (memoryStartAddress + (32 * arrayIndexStart + (31 - bitmapIndexStart)) * blockSize);
    }


    // handle errors
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
 * Free a one block of memory. It is important to notice
 * that only one block of size blockSize will be freed and not
 * the all the blocks that might have been allocated earlier.
 */
void BitmapMemoryManager::free(void *ptr) {
    uint32_t address = (uint32_t) ptr - memoryStartAddress;

    // check if pointer points to valid memory
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

    // set bit representing this block to 0
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
