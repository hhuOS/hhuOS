/*
 * Copyright (C) 2017-2025 Heinrich Heine University Düsseldorf,
 * Institute of Computer Science, Department Operating Systems
 * Main developers: Christian Gesse <christian.gesse@hhu.de>, Fabian Ruhland <ruhland@hhu.de>
 * Original development team: Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schöttner
 * This project has been supported by several students.
 * A full list of integrated student theses can be found here: https://github.com/hhuOS/hhuOS/wiki/Student-theses
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

#ifndef HHUOS_MEMORYSERVICE_H
#define HHUOS_MEMORYSERVICE_H

#include <stdint.h>

#include "Service.h"
#include "lib/util/base/String.h"
#include "lib/util/collection/ArrayList.h"
#include "device/bus/isa/Isa.h"
#include "kernel/memory/GlobalDescriptorTable.h"
#include "kernel/memory/Paging.h"
#include "kernel/memory/SlabAllocator.h"

namespace Kernel {
class PageFrameAllocator;
class PagingAreaManager;
}  // namespace Kernel

namespace Kernel {
class VirtualAddressSpace;

class MemoryService : public Service {

public:

    enum GlobalDescriptorTableType {
        KERNEL, BIOS_CALL
    };

    struct MemoryStatus {
        uint32_t totalPhysicalMemory;
        uint32_t freePhysicalMemory;
        uint32_t totalKernelHeapMemory;
        uint32_t freeKernelHeapMemory;
        uint32_t totalPagingAreaMemory;
        uint32_t freePagingAreaMemory;
    };

    /**
     * Constructor.
     */
    MemoryService(PageFrameAllocator *pageFrameAllocator, PagingAreaManager *pagingAreaManager, VirtualAddressSpace *kernelAddressSpace);

    /**
     * Copy Constructor.
     */
    MemoryService(const MemoryService &copy) = delete;

    /**
     * Assignment operator.
     */
    MemoryService& operator=(const MemoryService &other) = delete;

    /**
     * Destructor.
     */
    ~MemoryService() override;

    void* allocateKernelMemory(uint32_t size, uint32_t alignment = 0);

    void* reallocateKernelMemory(void *pointer, uint32_t size, uint32_t alignment = 0);

    void freeKernelMemory(void *pointer, uint32_t alignment = 0);

    void* allocateUserMemory(uint32_t size, uint32_t alignment = 0);

    void* reallocateUserMemory(void *pointer, uint32_t size, uint32_t alignment = 0);

    void freeUserMemory(void *pointer, uint32_t alignment = 0);

    void* allocateBiosMemory(uint32_t pageCount);

    void* allocateIsaMemory(uint32_t pageCount);

    void* allocatePhysicalMemory(uint32_t frameCount, void *startAddress = reinterpret_cast<void*>(Device::Isa::MAX_DMA_ADDRESS));

    void freePhysicalMemory(void *pointer, uint32_t frameCount);

    void* allocateKernelStack();

    void freeKernelStack(void *pointer);

    /**
     * Allocate space in PageTableArea.
     *
     * @return Address of the allocated page
     */
    Paging::Table* allocatePageTable();

    /**
     * Free a Page Table/Directory.
     *
     * @param pageTable Address of the table/directory that should be freed.
     */
    void freePageTable(Paging::Table *pageTable);

    /**
     * Maps a page into the current page directory at a given virtual address.
     *
     * @param virtualAddress Virtual address where a page should be mapped
     * @param flags Flags for Page Table Entry
     */
    void map(void *virtualAddress, uint32_t pageCount, uint16_t flags, bool abortIfLocked = false);

    /**
     * Unmap a page at a given virtual address.
     *
     * @param virtualAddress Virtual Address to be unmapped
     *
     * @return Physical Address of unmapped page
     */
    void* unmap(void *virtualAddress, uint32_t pageCount, uint32_t breakCount = 0);

    /**
     * Map a page at a given physical address to a virtual address.
     * The physical address should be allocated right now, since this function does only map it!
     *
     * @param virtualAddress Virtual address where a page should be mapped
     * @param flags Flags for Page Table entry
     * @param physicalAddress Physical address that should be mapped
     */
    void mapPhysical(void *physicalAddress, void *virtualAddress, uint32_t pageCount, uint16_t flags);

    /**
     * Map a physical address into the current address space's heap.
     * This is usually used for memory mapped IO (e.g. for the LFB).
     * The allocated memory is 4KB-aligned, therefore the returned virtual memory address is also 4KB-aligned.
     * If the given physical address is not 4KB-aligned, one has to add a offset to the returned virtual
     * memory address in order to obtain the corresponding virtual address.
     *
     * @param physicalAddress Physical address to be mapped. This address is usually given by a hardware device (e.g. for the LFB).
     *                 If the physical address lies in the address range of the installed physical memory of the system,
     *                 please make sure you allocated that memory before!
     * @param pageCount Amount of memory to be allocated
     *
     * @return Pointer to virtual memory block
     */
    void* mapIO(void *physicalAddress, uint32_t pageCount, bool mapToKernelHeap = true);

    /**
     * Allocate a contiguous block of physical memory and map it into the current address space's  heap.
     * This is useful for devices, which need memory for transfer operations.
     *
     * @param pageCount Amount of memory to be allocated
     *
     * @return Pointer to virtual memory block
     */
    void* mapIO(uint32_t pageCount, bool mapToKernelHeap = true);

    bool mapSharedMemory(uint32_t sourceProcessId, const Util::String &name, void *virtualAddress);

    /**
     * Get the physical address of a given virtual address. The returned physical address is 4 KiB aligned, so sometimes
     * an offset may be calculated in order to get the exact physical address corresponding to the virtual address.
     *
     * @param virtualAddress Virtual address
     *
     * @return Physical address of the given virtual address (4 KiB aligned)
     */
    void* getPhysicalAddress(void *virtualAddress);

    /**
     * Create a new virtual address space with its required memory managers.
     *
     * @return The new address space
     */
    VirtualAddressSpace& createAddressSpace();

    /**
     * Remove an address space from the system.
     *
     * @param addressSpace The address space to remove
     */
    void removeAddressSpace(VirtualAddressSpace &addressSpace);

    /**
     * Overriding function from InterruptHandler.
     */
    void handlePageFault(uint32_t errorCode);

    /**
     * Switch to a given address space.
     *
     * @param addressSpace The address space to switch to
     */
    void switchAddressSpace(VirtualAddressSpace &addressSpace);

    VirtualAddressSpace& getKernelAddressSpace() const;

    VirtualAddressSpace& getCurrentAddressSpace() const;

    const Util::ArrayList<VirtualAddressSpace*>& getAllAddressSpaces() const;

    MemoryStatus getMemoryStatus();

    void enableSlabAllocator();

    static const constexpr uint8_t SERVICE_ID = 2;

private:

    bool slabAllocatorEnabled = false;
    PageFrameAllocator &pageFrameAllocator;
    PagingAreaManager &pagingAreaManager;
    SlabAllocator pageFrameSlabAllocator;
    Util::BitmapMemoryManager kernelStackAllocator;

    Util::ArrayList<VirtualAddressSpace*> addressSpaces;
    VirtualAddressSpace *currentAddressSpace;
    VirtualAddressSpace &kernelAddressSpace;
};

}

#endif