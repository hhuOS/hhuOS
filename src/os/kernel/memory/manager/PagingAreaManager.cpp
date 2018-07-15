/* Paging Area Manager - manages the part of virtual memory resverved for
 * page tables and directories. A bitmap-based implementation.
 *
 * @author Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schoettner
 * @date HHU, 2018
 */

#include <kernel/memory/MemLayout.h>
#include "PagingAreaManager.h"
#include "kernel/memory/Paging.h"

/**
 * Constructor - calls base class with parameters.
 */
PagingAreaManager::PagingAreaManager()
        : BitmapMemoryManager(VIRT_PAGE_MEM_START, VIRT_PAGE_MEM_END, "PAGINGAREAMANAGER", true) {}

/**
 * Initializes the PageingAreaManager - sets up bitmap.
 */
void PagingAreaManager::init() {
    freeMemory = memoryEndAddress - memoryStartAddress;

    // calculate amount of pagetables/dirs that can be stored in the area
    uint32_t pageFrameCnt = freeMemory / (1024 * 4);
    // allocate bitmap for page frames
    freeBitmapLength = pageFrameCnt / 32;
    freeBitmap = new uint32_t[freeBitmapLength];

    memset(freeBitmap, 0, freeBitmapLength * sizeof(uint32_t));

    // We use already 256 Page Tables for Kernel mappings and one Page Directory
    // as the Kernel´s PD
    for(uint32_t i = 0; i < 8; i++){
        freeBitmap[i] = 0xFFFFFFFF;
    }
    freeBitmap[8] = 0xC0000000;

    // subtract already reserved memory from free memory
    freeMemory -= (8 * 32 * PAGESIZE + 2 * PAGESIZE);

    initialized = true;
}
