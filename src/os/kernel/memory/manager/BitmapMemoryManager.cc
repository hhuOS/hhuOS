//
// Created by burak on 02.03.18.
//

#include "BitmapMemoryManager.h"
#include "kernel/memory/Paging.h"
#include "lib/libc/printf.h"

extern "C" {
	#include "lib/libc/string.h"
}

BitmapMemoryManager::BitmapMemoryManager(uint32_t startAddress, uint32_t endAddress, String name, bool zeroMemory) : MemoryManager(startAddress, endAddress) {
    this->name = name;
    this->bmpIndex = 0;
    this->zeroMemory = zeroMemory;
}

uint32_t BitmapMemoryManager::alloc() {
// run through the bitmap-array
	bmpIndex=0;
    for(uint16_t i=0; i < freeBitMapLength; i++) {
    	if(i > 0){
			bmpIndex = (bmpIndex + 1) % freeBitMapLength;
		}
        // check if current entry of the array contains a free place (=0)
        if( ~freeBitMap[bmpIndex] != 0 ) {
            // calculate which page frame of the 32 is free
            uint8_t idx = 31;
            uint32_t tmpBitMap = ~freeBitMap[bmpIndex];
            // rotate the array entry to find the next 0-position
            do {
                if( (tmpBitMap & 0x80000000) != 0 ) {
                    // found free pageframe
                    break;
                }

                idx--;
                tmpBitMap <<= 1;
            } while(true);

            // set page frame as allocated in bitmap
            freeBitMap[bmpIndex] |= 1 << idx;
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
    // found no pageframe (TODO: interrupt)
    printf("[%s] KERNEL PANIC: Not enough memory", name);

    return 0;
}

uint32_t BitmapMemoryManager::free(uint32_t address) {
// check if kernel or i/o space is affected
    if(address < startAddress || address >= endAddress) {
#if DEBUG_BMM
        printf("[%s] ERROR: Something went wrong freeing the page - %x\n", name, address);
#endif
        return 0;
        // TODO: Bluescreen ?
    }

    address -= startAddress;

    // find number of page frame corresponding to physical address
    uint32_t pageFrameNumber = (uint32_t) (address / PAGESIZE);
    // calculate array idx to freeBitMap
    uint16_t arrayIdx = (uint16_t) (pageFrameNumber / 32);
    // calculate shift index
    uint8_t  idx = (uint8_t) (31 - (pageFrameNumber % 32));

    freeBitMap[arrayIdx] &= ~(1 << idx);
    freeMemory += PAGESIZE;

#if DEBUG_BMM
    printf("[%s] Free Page at address %x", name, address);
    printf(" Bitmap Index %d , Value_Offset %d\n", arrayIdx, (uint32_t)idx);
#endif

    return (address & 0xFFFFF000);
}
