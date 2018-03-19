/** 
 * PagingManager
 * 
 * Manages everything that has to do with paging. For example, the basic page
 * directory for kernel mappings is managed here. Furthermore, the Paging Manager
 * has to direct map-requests to the corresponding page directories.  
 * 
 * @author Burak Akguel, Christian Gesse
 * @date 2017
 */

#ifndef __SYSTEMMANAGEMENT_H__
#define __SYSTEMMANAGEMENT_H__

#include "kernel/interrupts/ISR.h"
#include "kernel/memory/PageDirectory.h"
#include "kernel/memory/manager/PageFrameAllocator.h"
#include "kernel/memory/manager/PagingAreaManager.h"
#include <stdint.h>
#include "kernel/memory/manager/HeapMemoryManager.h"
#include "kernel/memory/manager/IOMemoryManager.h"
#include "kernel/memory/VirtualAddressSpace.h"
#include "lib/util/ArrayList.h"

#define DEBUG_PM 0

// inherits from ISR, because page faults have to be addressed here
class SystemManagement : public ISR{

private:
    uint32_t faultFlags;
    uint32_t faultedAddress;

    static bool initialized;
    uint32_t totalPhysMemory = 0;

    // base page directory for for kernel mappings -> these mappings have to
    // appear in each process` page directory
    PageDirectory *basePageDirectory;
    VirtualAddressSpace *currentAddressSpace;
    // Page frame Allocator to alloc physical memory in 4kb-blocks
    PageFrameAllocator *pageFrameAllocator;
    // Paging Area Manager to manage the virtual memory reserved for page tables
    // and directories
    PagingAreaManager *pagingAreaManager;
    // IO memory manager
    IOMemoryManager *ioMemManager;

    Util::ArrayList<VirtualAddressSpace*> *addressSpaces;

    // is true if system runs in kernel mode (TODO: user mode needs to be implemented)
    static bool kernelMode;

    static SystemManagement *memoryManagement;
    static HeapMemoryManager *kernelMemoryManager;

public:
    SystemManagement() {};
    // init -> instead of constructor, because the Paging Manager is located
    // in the globals, but has to be initialized after system start
    void init();

    // plugin to handle interrupts
    void plugin();
    // handler for page faults
    void trigger();

    // Wrapper for access to IOMemoryManager
    IOMemInfo mapIO(uint32_t physAddress, uint32_t size);
    IOMemInfo mapIO(uint32_t size);
    IOMemInfo mapPhysRangeIO(uint32_t size);
    void freeIO(IOMemInfo memInfo);


    uint32_t allocPageTable();
    void freePageTable(uint32_t virtTableAddress);

    VirtualAddressSpace* createAddressSpace();
    void switchAddressSpace(VirtualAddressSpace *addressSpace);
    void removeAddressSpace(VirtualAddressSpace *addressSpace);

    // map function that redirects to corresponding page directory
    // different parameter sets for different requests
    void map(uint32_t virtAddress, uint16_t flags);
    void map(uint32_t virtAddress, uint16_t flags, uint32_t physAddress);
    void map(uint32_t virtStartAddress, uint32_t virtEndAddress, uint16_t flags);
    // create a page table in a given directory assuming that this table is
    // not present yet
    uint32_t createPageTable(PageDirectory *dir, uint32_t idx);
    // unmap a 4kb-page
    uint32_t unmap(uint32_t virtAddress);
    uint32_t unmap(uint32_t virtStartAddress, uint32_t virtEndAddress);

    // Obtain parameters of the last page fault
    void setFaultParams(uint32_t faultAddress, uint32_t flags);
    uint32_t getFaultingAddress();

    static bool isKernelMode();

    // get the physical address where virtAddr is mapped
    uint32_t getPhysicalAddress(uint32_t virtAddress);

    // print all free IO-memory blocks
    void dumpFreeIOMemBlocks(){
        ioMemManager->dump();
    }

    static bool isInitialized();

    void calcTotalPhysicalMemory();

    uint32_t getTotalPhysicalMemory() {
        return totalPhysMemory;
    }

    HeapMemoryManager* getCurrentUserSpaceHeapManager() {
    	return currentAddressSpace->getUserSpaceHeapManager();
    }

    static HeapMemoryManager* getKernelHeapManager() {
    	return kernelMemoryManager;
    }

    static SystemManagement* getInstance();
};

#endif
