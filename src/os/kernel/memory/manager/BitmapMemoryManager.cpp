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
BitmapMemoryManager::BitmapMemoryManager(uint32_t memoryStartAddress, uint32_t memoryEndAddress, String name, bool zeroMemory) : MemoryManager(memoryStartAddress, memoryEndAddress) {
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
            printf("[%s] Alloc Page at address %x\n", name, memoryStartAddress + (32 * bmpIndex + (31 - idx)) * PAGESIZE);
            printf(" Bitmap Index %d , Value_Offset %d" , i, (uint32_t)idx);
#endif
            freeMemory -= PAGESIZE;
            // return physical address of page frame
            if(zeroMemory){
                memset((void*)(memoryStartAddress + (32 * bmpIndex + (31 - idx)) * PAGESIZE), 0, PAGESIZE);
            }
            return (memoryStartAddress + (32 * bmpIndex + (31 - idx)) * PAGESIZE);
        }
    }
    // found no pageframe
    if(zeroMemory) {
        Cpu::throwException(Cpu::Exception ::OUT_OF_PAGE_MEMORY);
    } else {
        Cpu::throwException(Cpu::Exception ::OUT_OF_PHYS_MEMORY);
    }

    return 0;
}

/**
 * Free a 4kb memory block
 */
uint32_t BitmapMemoryManager::free(uint32_t address) {
// check if kernel or i/o space is affected
    if(address < memoryStartAddress || address >= memoryEndAddress) {
#if DEBUG_BMM
        printf("[%s] ERROR: Something went wrong freeing the page - %x\n", name, address);
#endif
        return 0;
    }

    address -= memoryStartAddress;

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
 * Reserves an address range of 4kb blocks and prevents from allocating it.
 * Can only be used on non-allocated memory.
 */
uint32_t BitmapMemoryManager::reserveAddressRange(uint32_t startAddress, uint32_t endAddress){
    // align addresses to 4kb layout
    uint32_t startAddr = startAddress & 0xFFFFF000;
    uint32_t endAddr = endAddress & 0xFFFFF000;
    endAddr += (endAddress % PAGESIZE == 0) ? 0 : PAGESIZE;

    // if requested memory is outside range, return
    if (startAddr < memoryStartAddress || endAddr > memoryEndAddress) {
        return 0;
    }

    // calculate number of pageframes
    uint32_t pageCnt = (endAddr  - startAddr) / PAGESIZE;
    // shift startAddr for use in Bitmap
    startAddr -= memoryStartAddress;

    // find number of frame corresponding to start address
    uint32_t startFrameNumber = (uint32_t) (startAddr / PAGESIZE);
    // calculate array idx to freeBitMap
    uint16_t startArrayIdx = (uint16_t) (startFrameNumber / 32);
    // calculate shift index for bitmask
    uint8_t  startIdx = (uint8_t) (startFrameNumber % 32);

    // set start parameters for loop
    uint32_t curPageCnt = 0;
    uint16_t arrayIdx = startArrayIdx;
    uint8_t idx = startIdx;

    // first loop: check if requested address range is free
    while(curPageCnt < pageCnt) {
        // if idx is 32, we need to choose the next entry of the bitmap
        if(idx == 32) {
            arrayIdx++;
            idx = 0;
        }

        // check if entry in bitmap is free
        if((freeBitmap[arrayIdx] & (0x80000000 >> idx)) != 0){
            // if a block is allocated, stop here and return 0
            return 0;
        }

        // increase counters
        idx++;
        curPageCnt++;
    }


    // reset parameters for loop
    curPageCnt = 0;
    arrayIdx = startArrayIdx;
    idx = startIdx;
    // second loop: mark address range as allocated
    while(curPageCnt < pageCnt) {
        // if idx is 32, we need to choose the next entry of the bitmap
        if(idx == 32) {
            arrayIdx++;
            idx = 0;
        }
        // set entry in bitmap to 1
        freeBitmap[arrayIdx] |= (0x80000000 >> idx);
        // increase counters
        idx++;
        curPageCnt++;
    }
    // update free memory
    freeMemory -= (pageCnt * PAGESIZE);
    // return start address of reserved block
    return (startAddr + memoryStartAddress);

}

/**
 * Frees an address range of 4kb blocks.
 */
void BitmapMemoryManager::freeAddressRange(uint32_t startAddress, uint32_t endAddress){
    // align addresses to 4kb layout
    uint32_t startAddr = startAddress & 0xFFFFF000;
    uint32_t endAddr = endAddress & 0xFFFFF000;
    endAddr += (endAddress % PAGESIZE == 0) ? 0 : PAGESIZE;

    // if requested memory is outside range, return
    if (startAddr < memoryStartAddress || endAddr > memoryEndAddress) {
        return;
    }

    // loop through addresses and free them
    for(uint32_t addr = startAddr; addr < endAddr; addr += PAGESIZE) {
        free(addr);
    }
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
