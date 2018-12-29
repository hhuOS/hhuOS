/*
 * Copyright (C) 2018 Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schoettner
 * Heinrich-Heine University
 *
 * This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any
 * later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>
 */

#ifndef __SYSTEMMANAGEMENT_H__
#define __SYSTEMMANAGEMENT_H__

#include "kernel/interrupts/InterruptHandler.h"
#include "kernel/memory/PageDirectory.h"
#include "kernel/memory/manager/PageFrameAllocator.h"
#include "kernel/memory/manager/PagingAreaManager.h"
#include "kernel/memory/manager/MemoryManager.h"
#include "kernel/memory/manager/IOMemoryManager.h"
#include "kernel/memory/VirtualAddressSpace.h"
#include "lib/util/ArrayList.h"

#define DEBUG_PM 0

/**
 * SystemManagement
 *
 * Is responsible for everything that has to do with address spaces and memory.
 * Keeps track of all registered address spaces and can dispatch memory requests and
 * mapping requests to the corresponding memory managers and page directories.
 *
 * @author Burak Akguel, Christian Gesse, Filip Krakowski, Fabian Ruhland, Michael Schoettner
 * @date 2018
 */
class SystemManagement : public InterruptHandler{

private:
	// parameters of a page fault
    uint32_t faultFlags{};
    uint32_t faultedAddress{};

    static bool initialized;
    // usable physical memory in this system
    uint32_t totalPhysMemory = 0;

    // base page directory for for kernel mappings -> these mappings have to
    // appear in each process` page directory
    PageDirectory *basePageDirectory{};
    // pointer to the currently active address space
    VirtualAddressSpace *currentAddressSpace{};
    // Page frame Allocator to alloc physical memory in 4kb-blocks
    PageFrameAllocator *pageFrameAllocator{};
    // Paging Area Manager to manage the virtual memory reserved for page tables
    // and directories
    PagingAreaManager *pagingAreaManager{};
    // IO memory manager
    IOMemoryManager *ioMemManager{};

    // list of all address spaces
    Util::ArrayList<VirtualAddressSpace*> *addressSpaces{};

    // is true if system runs in kernel mode (TODO: user mode needs to be implemented)
    static bool kernelMode;

    //
    static SystemManagement *systemManagement;
    static MemoryManager *kernelMemoryManager;

public:

    /**
     * Constructor.
     */
    SystemManagement() = default;

    /**
     * Initialize the SystemManager and the corresponding stuff
     */
    void init();

    // page fault handling

    /**
     * Plugin to register for interrupt handling
     */
    void plugin();

    /**
     * Handle a Page Fault
     */
    void trigger(InterruptFrame &frame) override;

    /**
	 * Sets the params for a page fault.
	 *
	 * @param faultAddress The address where the page fault occured
	 * @param flags Flags of the last page fault
	 */
	void setFaultParams(uint32_t faultAddress, uint32_t flags);

	/**
	 * Returns the faulting address of the last pagefault that occured.
	 *
	 * @return Last faulting address
	 */
	uint32_t getFaultingAddress();

    // Wrappers for access to IOMemoryManager

    /**
     * Maps a physical address into the IO-space of the system, located at the upper
     * end of the virtual memory. The allocated memory is 4kb-aligned, therefore the
     * returned virtual memory address is also 4kb-aligned. If the given physical
     * address is not 4kb-aligned, one has to add a offset to the returned virtual
     * memory address in order to obtain the corresponding virtual address.
     *
     * @param physAddr Physical address to be mapped. This address is usually given
     *                 by a hardware device (e.g. for the LFB). If the pyhsAddr lies
     *                 in the address rang of the installed physical memory of the system,
     *                 please make sure you allocated that memory before!
     * @param size Size of memory to be allocated
     * @return Pointer to virtual IO memory block
     */
    void * mapIO(uint32_t physAddress, uint32_t size);

    /**
     * Maps IO-space for a device and allocates physical memory for it. All
     * allocations are 4kb-aligned.
     *
     * @param size Size of IO-memory to be allocated
     * @return Pointer to virtual IO memory block
     */
    void * mapIO(uint32_t size);

    /**
     * Free the IO-space described by the given IOMemInfo Block
     *
     * @param ptr Pointer to virtual IO memory that should be freed
     */
    void freeIO(void *ptr);


    // allocations and deletions of page tables

    /**
     * Allocates space in PageTableArea.
     *
     * @return Adress of the allocated page
     */
    void * allocPageTable();

    /**
     * Frees a Page Table / Directory.
     *
     * @param virtTableAddress Address of the table/directory that should be freed.
     */
    void freePageTable(void *virtTableAddress);

    /**
     * Creates Page Table for a non present entry in Page Directory
     *
     * @param dir Page Directory where Table should be mapped
     * @param idx Index into the Page Directory
     * @return uint32_t Return Value
     */
    uint32_t createPageTable(PageDirectory *dir, uint32_t idx);


    // address space management

    /**
     * Creates a new virtual address space and the required memory managers.
     *
     * @return Pointer to the new address space
     */
    VirtualAddressSpace* createAddressSpace();

    /**
     * Switches to a given address space.
     *
     * @param addressSpace Pointer to the address spaces that should be switched to
     */
    void switchAddressSpace(VirtualAddressSpace *addressSpace);

    /**
     * Remove an address space from the system
     *
     * @param adressSpace AdressSpace that should be removed
     */
    void removeAddressSpace(VirtualAddressSpace *addressSpace);


    // Mappings and unmappings

    /**
     * Maps a page into the current page directory at a given virtual address.
     *
     * @param virtAddress Virtual address where a page should be mapped
     * @param flags Flags for Page Table Entry
     */
    void map(uint32_t virtAddress, uint16_t flags);

    /**
     * Maps a page at a given physical address to a virtual address.
     * The physical address should be allocated right now, since this function does
     * only map it!
     *
     * @param virtAddress Virtual address where a page should be mapped
     * @param flags Flags for Page Table entry
     * @param physAddress Physical address that should be mapped
     */
    void map(uint32_t virtAddress, uint16_t flags, uint32_t physAddress);

    /**
     * Range map function to map a range of virtual addresses into the current Page
     * Directory .
     *
     * @param virtStartAddress Virtual start address of the mapping
     * @param virtEndAddress Virtual start address of the mapping
     * @param flags Flags for the Page Table entries
     */
    void map(uint32_t virtStartAddress, uint32_t virtEndAddress, uint16_t flags);

    /**
     * Unmap Page at a given virtual address.
     *
     * @param virtAddress Virtual Address to be unmapped
     * @return uint32_t Physical Address of unmapped page
     */
    uint32_t unmap(uint32_t virtAddress);

    /**
     * Unmap a range of virtual addresses in current Page Directory
     *
     * @param startVirtAddress Virtual start address to be unmapped
     * @param endVirtAddress last address to be unmapped
     * @return uint32_t Physical address of the last unmapped page
     */
    uint32_t unmap(uint32_t virtStartAddress, uint32_t virtEndAddress);


    /**
     * Checks whether the system is in kernel mode or not.
     * Is needed to decide which heap memory manager is to be used.
     *
     * @return true= kernel mode, false = user mode
     */
    static bool isKernelMode();

    /**
     * Protects kernel pages from being written on.
     */
    void writeProtectKernelCode();

    /**
     * Gets the physical address of a given virtual address. The returned
     * physical address is 4kb-aligned, so sometimes an offset may be calculated
     * in order to get the exact physical address corresponding to the virtual
     * address.
     *
     * @param virtAddr Virtual address
     * @return uint32_t Physical address of the given virtual address (4kb-aligned)
     */
    void * getPhysicalAddress(void *virtAddress);

    /**
     * Dumps all free IO Memory blocks.
     */
    void dumpFreeIOMemBlocks(){
        ioMemManager->dump();
    }

    /**
     * Checks if the system management is fully initialized.
     *
     * @return State of the SystemManagement
     */
    static bool isInitialized();

    static void createGDTEntry(uint16_t* gdt, uint16_t num, uint32_t base, uint32_t limit, uint8_t access, uint8_t flags);

    /**
     * Calculates the amount of usable, installed physical memory using a bios call.
     *
     * @return Amount of usable physical memory
     */
    void calcTotalPhysicalMemory();

    uint32_t getTotalPhysicalMemory() {
        return totalPhysMemory;
    }

    /**
     * Returns the current userspace memory manager.
     *
     * @return Pointer to the current userspace memory manager
     */
    MemoryManager* getCurrentUserSpaceHeapManager() {
    	return currentAddressSpace->getUserSpaceHeapManager();
    }

    /**
	 * Returns the current kernelspace memory manager.
	 *
	 * @return Pointer to the current kernelspace memory manager
	 */
    static MemoryManager* getKernelHeapManager() {
    	return kernelMemoryManager;
    }

    /**
     * Getter method for the singleton-construction.
     * Creates an instance if necessary.
     *
     * @return Pointer to the instance of the SystemManagement
     */
    static SystemManagement & getInstance();

    IOMemoryManager* getIOMemoryManager() {
        return ioMemManager;
    }

    PageFrameAllocator* getPageFrameAllocator() {
        return pageFrameAllocator;
    }

    PagingAreaManager* getPagingAreaManager() {
        return pagingAreaManager;
    }

    void *realloc(void *ptr, uint32_t size, uint32_t alignment = 0);
};

void* operator new(size_t size, uint32_t alignment);

void* operator new[](size_t size, uint32_t alignment);

void operator delete(void *ptr, uint32_t alignment);

void operator delete[](void *ptr, uint32_t alignment);

#endif
