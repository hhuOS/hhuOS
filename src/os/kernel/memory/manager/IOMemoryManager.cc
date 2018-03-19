/* IOMemoryManager - Manages IO-Space for HW-Buffers, DMA etc.
*  Maps a given physical address (for example LFB) into the virtual IO-space
*  as defined in MemLayout.h or allocates an 4kb-aligned physical buffer and 
*  maps it into virtual IO-space.
*
*  Authors: Burak Akgül and Christian Gesse, HHU 2017  */

#include <kernel/memory/MemLayout.h>
#include <lib/libc/printf.h>
#include "IOMemoryManager.h"

#include "../SystemManagement.h"

#include "kernel/memory/Paging.h"

extern "C" {
    #include "lib/libc/string.h"
}

IOMemoryManager::IOMemoryManager() : MemoryManager(VIRT_IO_START, VIRT_IO_END) {

}

void IOMemoryManager::init(){
    freeMemory = endAddress - startAddress;
    // start of memory area -> anchor of free list
    anchor = (IOMemFreeHeader*) (startAddress);
    anchor->next = 0;
    anchor->prev = 0;
    anchor->pageCount = freeMemory/PAGESIZE;
}

IOMemInfo IOMemoryManager::alloc(uint32_t* physAddresses, uint32_t pageCnt){
    IOMemFreeHeader* tmp = anchor;

#if DEBUG_IOMEM
    printf("Mapped IO" << endl;
#endif

    while(tmp){
        if(tmp->pageCount >= pageCnt){
            if(tmp->pageCount == pageCnt){
                if(tmp == anchor){
                    anchor = tmp->next;
                    anchor->prev = 0;
                } else {
                    if(tmp->next) {
                        tmp->next->prev = tmp->prev;
                    }
                    tmp->prev->next = tmp->next;
                }
            } else {
                IOMemFreeHeader* newHeader = (IOMemFreeHeader*) ((uint32_t)tmp + pageCnt*PAGESIZE);
                
                if(!tmp->prev) {
                    anchor = newHeader;
                    anchor->prev = 0;
                } else {
                    newHeader->prev = tmp->prev;
                    tmp->prev->next = newHeader;
                }

                newHeader->next = tmp->next;
                newHeader->pageCount = tmp->pageCount - pageCnt;
                if(tmp->next) {
                    tmp->next->prev = newHeader;
                }
            }
            
            for(uint32_t i = 0; i < pageCnt; i++){
                uint32_t vaddr = (uint32_t)tmp + i * PAGESIZE;
                SystemManagement::getInstance()->unmap(vaddr);
                SystemManagement::getInstance()->map(vaddr, PAGE_READ_WRITE|PAGE_PRESENT|PAGE_NO_CACHING, physAddresses[i]);
            }

            IOMemInfo ret = (IOMemInfo) {};
            ret.virtStartAddress = (uint32_t) tmp;
            ret.pageCount = pageCnt;
            ret.physAddresses = physAddresses;
            freeMemory -= (pageCnt * PAGESIZE);
            return ret;
        }
        tmp = tmp->next;
    }
    return (IOMemInfo) {0,0,0};
}

void IOMemoryManager::free(IOMemInfo memInfo){
    uint32_t virtStart = memInfo.virtStartAddress;
    uint32_t pageCnt = memInfo.pageCount;

    freeMemory += (pageCnt * PAGESIZE);

    IOMemFreeHeader* tmp = new IOMemFreeHeader;
    memset((void*) tmp, 0, sizeof(IOMemFreeHeader));
    uint32_t virtHeaderAddress = 0;
    if(virtStart < (uint32_t)anchor) {
        virtHeaderAddress = (uint32_t) virtStart;
        tmp->pageCount = pageCnt;
        tmp->prev = 0;
        tmp->next = anchor;
        memcpy((void*) virtHeaderAddress, tmp, sizeof(IOMemFreeHeader));
        anchor->prev = (IOMemFreeHeader*) virtHeaderAddress;
        anchor = anchor->prev;
    } else if(anchor == 0) { // virt io area was completely full
        virtHeaderAddress = (uint32_t) virtStart;
        tmp->pageCount = pageCnt;
        tmp->prev = 0;
        tmp->next = 0;
        memcpy((void*) virtHeaderAddress, tmp, sizeof(IOMemFreeHeader));
        anchor = (IOMemFreeHeader*) virtHeaderAddress;
    } else {
        IOMemFreeHeader* prev = anchor;
        while((uint32_t)prev->next < virtStart) {
            prev = prev->next;
        }

        virtHeaderAddress = (uint32_t) virtStart;
        tmp->pageCount = pageCnt;
        tmp->prev = prev;
        tmp->next = prev->next;

        memcpy((void*) virtHeaderAddress, tmp, sizeof(IOMemFreeHeader));

        prev->next->prev = (IOMemFreeHeader*) virtHeaderAddress;
        prev->next = (IOMemFreeHeader*) virtHeaderAddress;
    }

    // Merge with next if possible
    if(tmp->next && virtStart + PAGESIZE * pageCnt == (uint32_t)tmp->next) {
        tmp->pageCount += tmp->next->pageCount;

        tmp->next = tmp->next->next;
        if(tmp->next) {
                tmp->next->prev = (IOMemFreeHeader*) virtHeaderAddress;
        }
    }
    // Merge with prev if possible
    if(tmp->prev && (uint32_t)tmp->prev + PAGESIZE * tmp->prev->pageCount == virtStart) {
        tmp->pageCount += tmp->prev->pageCount;

        virtHeaderAddress = (uint32_t) tmp->prev;

        tmp->prev = tmp->prev->prev;

        if(tmp->next) {
            tmp->next->prev = (IOMemFreeHeader*) virtHeaderAddress;
        }
    }

    uint32_t* physAddresses = memInfo.physAddresses;
    for(uint32_t i=0; i < pageCnt; i++) {
        uint32_t freedPhysAddress = SystemManagement::getInstance()->unmap(virtStart + i*PAGESIZE);
        if(physAddresses[i] < SystemManagement::getInstance()->getTotalPhysicalMemory() && physAddresses[i] != freedPhysAddress) {
            printf("[IOMEMMANAGER] Unmap failed for physical address %x\n", physAddresses[i]);
        }
    }

    IOMemFreeHeader *tmp2 = (IOMemFreeHeader*) virtHeaderAddress;
    memcpy(tmp2, tmp, sizeof(IOMemFreeHeader));
    delete tmp;

}

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
