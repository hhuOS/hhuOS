/* Paging Area Manager - manages the part of virtual memory resverved for 
 * page tables and directories. A bitmap-based implementation.
 * 
 * Burak Akguel, Christian Gesse, HHU 2017 */

#include <kernel/memory/MemLayout.h>
#include "PagingAreaManager.h"



PagingAreaManager::PagingAreaManager()
        : BitmapMemoryManager(VIRT_PAGE_MEM_START, VIRT_PAGE_MEM_END, "PAGINGAREAMANAGER", true) {}

void PagingAreaManager::init() {
    freeMemory = endAddress - startAddress;

    // calculate amount of pagetables/dirs that can be stored in the area
    uint32_t pageFrameCnt = freeMemory / (1024 * 4);
    // allocate bitmap for page frames
    freeBitMapLength = pageFrameCnt / 32;
    freeBitMap = new uint32_t[freeBitMapLength];

    // We use already 256 Page Tables for Kernel mappings and one Page Directory
    // as the Kernel´s PD
    for(uint32_t i = 0; i < 8; i++){
        freeBitMap[i] = 0xFFFFFFFF;
    }
    freeBitMap[8] = 0xC0000000;

    initialized = true;
}
