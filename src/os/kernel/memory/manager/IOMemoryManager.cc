/* IOMemoryManager - Manages IO-Space for HW-Buffers, DMA etc.
*  Maps a given physical address (for example LFB) into the virtual IO-space
*  as defined in MemLayout.h or allocates an 4kb-aligned physical buffer and
*  maps it into virtual IO-space.
*
 * @author Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schoettner
 * @date HHU, 2018
 */

#include <kernel/memory/MemLayout.h>
#include <lib/libc/printf.h>
#include "IOMemoryManager.h"

#include "../SystemManagement.h"

#include "kernel/memory/Paging.h"

extern "C" {
    #include "lib/libc/string.h"
}

/**
 * Constructor - call constructor of base class.
 */
IOMemoryManager::IOMemoryManager() : MemoryManager(VIRT_IO_START, VIRT_IO_END) {

}

/**
 * Initialize memory manager and set up free list.
 */
void IOMemoryManager::init(){
    freeMemory = endAddress - startAddress;
    // start of memory area -> create anchor of free list
    anchor = (IOMemFreeHeader*) (startAddress);
    anchor->next = 0;
    anchor->prev = 0;
    anchor->pageCount = freeMemory/PAGESIZE;
}

/**
 * Allocate some virtual 4kb pages for given physical page frames.
 */
IOMemInfo IOMemoryManager::alloc(uint32_t* physAddresses, uint32_t pageCnt){
	// use pointer to iterate thrugh list
    IOMemFreeHeader* tmp = anchor;

    // iterate
    while(tmp){
    	// we found a free block with enough pages
        if(tmp->pageCount >= pageCnt){
        	// if block matches the requested size exactly, brach here
            if(tmp->pageCount == pageCnt){
            	// if block was also anchor for the free list, the next header will be anchor
                if(tmp == anchor){
                    anchor = tmp->next;
                    anchor->prev = 0;
                // else we can simply remove this block from the free list
                } else {
                    if(tmp->next) {
                        tmp->next->prev = tmp->prev;
                    }
                    tmp->prev->next = tmp->next;
                }
            // if block has more pages than requested, we split into 2 blocks
            } else {
            	// create header for new second block
                IOMemFreeHeader* newHeader = (IOMemFreeHeader*) ((uint32_t)tmp + pageCnt*PAGESIZE);
                
                // check if new header will be anchor of the free list
                // and update previous pointers
                if(!tmp->prev) {
                    anchor = newHeader;
                    anchor->prev = 0;
                } else {
                    newHeader->prev = tmp->prev;
                    tmp->prev->next = newHeader;
                }

                // update next pointers
                newHeader->next = tmp->next;
                // set size of new block
                newHeader->pageCount = tmp->pageCount - pageCnt;
                // update previous pointer of next block if it exists
                if(tmp->next) {
                    tmp->next->prev = newHeader;
                }
            }
            
            // map the allocated virtual pages to the given phyiscal addresses
            for(uint32_t i = 0; i < pageCnt; i++){
            	// since the virtual memory is one block, we can update the virtual address this way
                uint32_t vaddr = (uint32_t)tmp + i * PAGESIZE;

                // if the virtual address is already mapped, we have to unmap it
                // this can happen becuase the headers of the free list are mapped
                // to arbitrary physical addresses, but the IO Memory should be mapped
                // to given phyiscal addresses
                SystemManagement::getInstance()->unmap(vaddr);
                // map the page to given phyiscal address
                SystemManagement::getInstance()->map(vaddr, PAGE_READ_WRITE|PAGE_PRESENT|PAGE_NO_CACHING, physAddresses[i]);
            }

            // create IOMemInfo strcut for return
            IOMemInfo ret = (IOMemInfo) {};
            ret.virtStartAddress = (uint32_t) tmp;
            ret.pageCount = pageCnt;
            ret.physAddresses = physAddresses;
            // update free memory
            freeMemory -= (pageCnt * PAGESIZE);
            // return
            return ret;
        }
        // look for next memory block
        tmp = tmp->next;
    }
    // if the loop is passed without returning, no memory block with enough pages is free
    return (IOMemInfo) {0,0,0};
}

/**
 * Free virtual IO memory.
 *
 * @param memInfo IOMemInfo strcut with all information regarding the memory block
 */
void IOMemoryManager::free(IOMemInfo memInfo){
	// get important parameters
    uint32_t virtStart = memInfo.virtStartAddress;
    uint32_t pageCnt = memInfo.pageCount;

    // catch error
    if(virtStart < startAddress || virtStart >= endAddress){
    	return;
    }

    freeMemory += (pageCnt * PAGESIZE);

    /*
     * The next steps are a bit complicated we create a new temporary header for the free list in heap
     * and fill it with the new values.
     * Afterwards, we unmap the IO memory (since it could be pointing to hardware buffers) and copy
     * the header to the begin of the memory block. This causes a page fault and the header
     * will be mapped to an arbitrary phyiscal address
     */

    // create temporary header
    IOMemFreeHeader* tmp = new IOMemFreeHeader;
    // zero it out
    memset((void*) tmp, 0, sizeof(IOMemFreeHeader));
    // the virtual address where this header should be placed is calculated later
    uint32_t virtHeaderAddress = 0;
    // if we found a new anchor, branch here
    if(virtStart < (uint32_t)anchor) {
    	// we have new anchor at the beginning of this memory block
        virtHeaderAddress = (uint32_t) virtStart;
        // update pointer and vaules
        tmp->pageCount = pageCnt;
        tmp->prev = 0;
        tmp->next = anchor;
        // copy header to the correct position -> will cause a pagefault and map the page
        memcpy((void*) virtHeaderAddress, tmp, sizeof(IOMemFreeHeader));
        // update current anchor
        anchor->prev = (IOMemFreeHeader*) virtHeaderAddress;
        // set new anchor
        anchor = anchor->prev;
    // if all IO memory was allocated before, we have to start a new free list
    } else if(anchor == 0) {
    	// create a new anchor for a new free list
        virtHeaderAddress = (uint32_t) virtStart;
        tmp->pageCount = pageCnt;
        tmp->prev = 0;
        tmp->next = 0;
        // copy header to right position
        memcpy((void*) virtHeaderAddress, tmp, sizeof(IOMemFreeHeader));
        anchor = (IOMemFreeHeader*) virtHeaderAddress;
    // else we can simply update the free list
    } else {
    	// get the previous free memory block of this memory block
        IOMemFreeHeader* prev = anchor;
        while((uint32_t)prev->next < virtStart) {
            prev = prev->next;
        }

        // set values in temporary header
        virtHeaderAddress = (uint32_t) virtStart;
        tmp->pageCount = pageCnt;
        tmp->prev = prev;
        tmp->next = prev->next;

        // copy the new header to the correct position and map it (through pagefault)
        memcpy((void*) virtHeaderAddress, tmp, sizeof(IOMemFreeHeader));

        // set pointer right
        prev->next->prev = (IOMemFreeHeader*) virtHeaderAddress;
        prev->next = (IOMemFreeHeader*) virtHeaderAddress;
    }

    // for merging we use the temporary header on the heap

    // Merge with next if possible and update tmp
    if(tmp->next && virtStart + PAGESIZE * pageCnt == (uint32_t)tmp->next) {
        tmp->pageCount += tmp->next->pageCount;

        tmp->next = tmp->next->next;
        if(tmp->next) {
                tmp->next->prev = (IOMemFreeHeader*) virtHeaderAddress;
        }
    }
    // Merge with prev if possible and update tmp
    if(tmp->prev && (uint32_t)tmp->prev + PAGESIZE * tmp->prev->pageCount == virtStart) {
        tmp->pageCount += tmp->prev->pageCount;

        // the virtual address of the tmp header has moved
        virtHeaderAddress = (uint32_t) tmp->prev;

        tmp->prev = tmp->prev->prev;

        if(tmp->next) {
            tmp->next->prev = (IOMemFreeHeader*) virtHeaderAddress;
        }
    }

    // unmap the whole merged block of virtual memory
    for(uint32_t i=0; i < tmp->pageCount; i++) {
        SystemManagement::getInstance()->unmap(virtHeaderAddress + i*PAGESIZE);
    }

    // copy the memory block to the correct position
    // header will be mapped through pagefault
    IOMemFreeHeader *tmp2 = (IOMemFreeHeader*) virtHeaderAddress;
    memcpy(tmp2, tmp, sizeof(IOMemFreeHeader));
    delete tmp;

}

/**
 * Print dump of the free list.
 */
void IOMemoryManager::dump(){
    IOMemFreeHeader* tmp = anchor;
    printf("Dump of free IO-memory blocks\n\n");
    printf("Start\tPageCnt\tPrev\tNext\n\n");
    printf("-------------------------------------------------------------\n");
    while(tmp){
        printf("%x\t%d\t%x\t%x\n", (uint32_t) tmp , tmp->pageCount, (uint32_t)tmp->prev , (uint32_t)tmp->next);
        tmp = tmp->next;
    }
    printf("-------------------------------------------------------------\n");
}
