/**
 * BitmapMemoryManager - manages a given area of memory in 4kb blocks using
 * a bitmap mechanism.
 *
 * @author Burak Akguel, Christian Gesse, Filip Krakowski, Fabian Ruhland, Michael Schoettner
 * @date 2018
 */

#include "BitmapMemoryManager.h"
#include "kernel/memory/Paging.h"
#include "lib/libc/printf.h"

extern "C" {
	#include "lib/libc/string.h"
}

/**
 * Constructor
 */
BitmapMemoryManager::BitmapMemoryManager(uint32_t startAddress, uint32_t endAddress, String name, bool zeroMemory) : MemoryManager(startAddress, endAddress) {
    this->name = name;
    this->bmpIndex = 0;
    this->zeroMemory = zeroMemory;
}

/**
 * Allocate a 4kb block of memory
 */
uint32_t BitmapMemoryManager::alloc() {
	// run through the bitmap-array
	bmpIndex=0;
    for(uint16_t i=0; i < freeBitmapLength; i++) {
    	// increase bmpIndex
    	if(i > 0){
			bmpIndex = (bmpIndex + 1) % freeBitmapLength;
		}
        // check if current entry of the array contains a free place (=0)
        if( ~freeBitmap[bmpIndex] != 0 ) {
            // calculate which page frame of the 32 is free
            uint8_t idx = 31;
            uint32_t tmpBitMap = ~freeBitmap[bmpIndex];
            // rotate the array entry to find the next 0-position
            do {
            	// we want to see if the page represented by the highest bit is free (=0)
                if( (tmpBitMap & 0x80000000) != 0 ) {
                    // found free pageframe
                    break;
                }

                idx--;
                // shift current tmpBitmap to get next bit to the highest position
                tmpBitMap <<= 1;
            } while(true);

            // set page frame as allocated in bitmap
            freeBitmap[bmpIndex] |= 1 << idx;
#if DEBUG_BMM
            printf("[%s] Alloc Page at address %x\n", name, startAddress + (32 * bmpIndex + (31 - idx)) * PAGESIZE);
            printf(" Bitmap Index %d , Value_Offset %d" , i, (uint32_t)idx);
#endif
            freeMemory -= PAGESIZE;
            // return physical address of page frame
            if(zeroMemory){
            	memset((void*)(startAddress + (32 * bmpIndex + (31 - idx)) * PAGESIZE), 0, PAGESIZE);
            }
            return (startAddress + (32 * bmpIndex + (31 - idx)) * PAGESIZE);
        }
    }
    // found no pageframe
    printf("[%s] KERNEL PANIC: Not enough memory", name);

    return 0;
}

/**
 * Free a 4kb memory block
 */
uint32_t BitmapMemoryManager::free(uint32_t address) {
// check if kernel or i/o space is affected
    if(address < startAddress || address >= endAddress) {
#if DEBUG_BMM
        printf("[%s] ERROR: Something went wrong freeing the page - %x\n", name, address);
#endif
        return 0;
    }

    address -= startAddress;

    // find number of page frame corresponding to physical address
    uint32_t pageFrameNumber = (uint32_t) (address / PAGESIZE);
    // calculate array idx to freeBitMap
    uint16_t arrayIdx = (uint16_t) (pageFrameNumber / 32);
    // calculate shift index
    uint8_t  idx = (uint8_t) (31 - (pageFrameNumber % 32));

    // set bit representing this page to 0
    freeBitmap[arrayIdx] &= ~(1 << idx);
    freeMemory += PAGESIZE;

#if DEBUG_BMM
    printf("[%s] Free Page at address %x", name, address);
    printf(" Bitmap Index %d , Value_Offset %d\n", arrayIdx, (uint32_t)idx);
#endif

    // return the 4kb aligned address that was freed
    return (address & 0xFFFFF000);
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
