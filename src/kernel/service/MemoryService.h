/*
 * Copyright (C) 2018-2022 Heinrich-Heine-Universitaet Duesseldorf,
 * Institute of Computer Science, Department Operating Systems
 * Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schoettner
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

#include <cstdint>

#include "lib/util/stream/OutputStreamWriter.h"
#include "Service.h"
#include "device/port/serial/SerialPort.h"
#include "kernel/interrupt/InterruptHandler.h"
#include "lib/util/data/Array.h"
#include "lib/util/data/ArrayList.h"
#include "lib/util/data/Collection.h"
#include "lib/util/data/Iterator.h"
#include "lib/util/memory/FreeListMemoryManager.h"

namespace Kernel {
class PageDirectory;
class PageFrameAllocator;
class PagingAreaManager;
class VirtualAddressSpace;
struct InterruptFrame;
}  // namespace Kernel

#define HHUOS_MEMORYSERVICE_ENABLE_DEBUG 0

namespace Kernel {

class MemoryService : public Service, public InterruptHandler {

public:

    struct MemoryStatus {
        uint32_t totalPhysicalMemory;
        uint32_t freePhysicalMemory;
        uint32_t totalLowerMemory;
        uint32_t freeLowerMemory;
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

    void *reallocateKernelMemory(void *pointer, uint32_t size, uint32_t alignment = 0);

    void freeKernelMemory(void *pointer, uint32_t alignment = 0);

    void* allocateUserMemory(uint32_t size, uint32_t alignment = 0);

    void *reallocateUserMemory(void *pointer, uint32_t size, uint32_t alignment = 0);

    void freeUserMemory(void *pointer, uint32_t alignment = 0);

    void* allocateLowerMemory(uint32_t size, uint32_t alignment = 0);

    void* reallocateLowerMemory(void *pointer, uint32_t size, uint32_t alignment = 0);

    void freeLowerMemory(void *pointer, uint32_t alignment = 0);

    /**
     * Allocate space in PageTableArea.
     *
     * @return Address of the allocated page
     */
    void* allocatePageTable();

    /**
     * Free a Page Table/Directory.
     *
     * @param virtualTableAddress Address of the table/directory that should be freed.
     */
    void freePageTable(void *virtualTableAddress);

    /**
     * Create Page Table for a non present entry in Page Directory.
     *
     * @param directory Page Directory where Table should be mapped
     * @param index Index into the Page Directory
     */
    void createPageTable(PageDirectory *directory, uint32_t index);

    /**
     * Map a page at a given physical address to a virtual address.
     * The physical address should be allocated right now, since this function does only map it!
     *
     * @param virtualAddress Virtual address where a page should be mapped
     * @param flags Flags for Page Table entry
     * @param physicalAddress Physical address that should be mapped
     */
    void mapPhysicalAddress(uint32_t virtualAddress, uint32_t physicalAddress, uint16_t flags);

    /**
     * Map a range of virtual addresses into the current Page Directory.
     *
     * @param virtualStartAddress Virtual start address of the mapping
     * @param virtualEndAddress Virtual start address of the mapping
     * @param flags Flags for the Page Table entries
     */
    void mapRange(uint32_t virtualStartAddress, uint32_t virtualEndAddress, uint16_t flags);

    /**
     * Maps a page into the current page directory at a given virtual address.
     *
     * @param virtualAddress Virtual address where a page should be mapped
     * @param flags Flags for Page Table Entry
     */
    void map(uint32_t virtualAddress, uint16_t flags);

    /**
     * Map a physical address into the current address space's heap.
     * This is usually used to map physical TransferMode memory (e.g. for the LFB) into a virtual address space and be able to access it.
     * The allocated memory is 4KB-aligned, therefore the returned virtual memory address is also 4KB-aligned.
     * If the given physical address is not 4KB-aligned, one has to add a offset to the returned virtual
     * memory address in order to obtain the corresponding virtual address.
     *
     * @param physicalAddress Physical address to be mapped. This address is usually given by a hardware device (e.g. for the LFB).
     *                 If the physical address lies in the address range of the installed physical memory of the system,
     *                 please make sure you allocated that memory before!
     * @param size Amount of memory to be allocated
     *
     * @return Pointer to virtual TransferMode memory block
     */
    void *mapIO(uint32_t physicalAddress, uint32_t size, bool mapToKernelHeap = true);

    /**
     * Allocate a contiguous block of physical memory and map it into the current address space's  heap.
     * This is useful for devices, which need memory for TransferMode operations.
     *
     * @param size Amount of memory to be allocated
     *
     * @return Pointer to virtual TransferMode memory block
     */
    void *mapIO(uint32_t size, bool mapToKernelHeap = true);

    /**
     * Unmap a page at a given virtual address.
     *
     * @param virtualAddress Virtual Address to be unmapped
     *
     * @return Physical Address of unmapped page
     */
    uint32_t unmap(uint32_t virtualAddress);

    /**
     * Unmap a range of virtual addresses in the current page directory
     *
     * @param startVirtAddress Virtual start address to be unmapped
     * @param endVirtAddress last address to be unmapped
     *
     * @return Physical address of the last unmapped page
     */
    uint32_t unmap(uint32_t virtualStartAddress, uint32_t virtualEndAddress, uint32_t breakCount = 0);

    /**
     * Get the physical address of a given virtual address. The returned physical address is 4 KiB aligned, so sometimes
     * an offset may be calculated in order to get the exact physical address corresponding to the virtual address.
     *
     * @param virtualAddress Virtual address
     *
     * @return Physical address of the given virtual address (4 KiB aligned)
     */
    void *getPhysicalAddress(void *virtualAddress);

    /**
     * Create a new virtual address space with its required memory managers.
     *
     * @return The new address space
     */
    VirtualAddressSpace &createAddressSpace();

    /**
     * Switch to a given address space.
     *
     * @param addressSpace The address space to switch to
     */
    void switchAddressSpace(VirtualAddressSpace &addressSpace);

    /**
     * Remove an address space from the system.
     *
     * @param addressSpace The address space to remove
     */
    void removeAddressSpace(VirtualAddressSpace &addressSpace);

    /**
     * Overriding function from InterruptHandler.
     */
    void plugin() override;

    /**
     * Overriding function from InterruptHandler.
     */
    void trigger(const Kernel::InterruptFrame &frame) override;

    [[nodiscard]] VirtualAddressSpace& getKernelAddressSpace() const;

    [[nodiscard]] VirtualAddressSpace& getCurrentAddressSpace() const;

    MemoryStatus getMemoryStatus();

    void setSerialDebugging(bool enabled);

    void printDebugNumber(uint32_t number, uint8_t base);

    static const constexpr uint8_t SERVICE_ID = 2;

private:

    Util::Memory::FreeListMemoryManager lowerMemoryManager;

    PageFrameAllocator &pageFrameAllocator;
    PagingAreaManager &pagingAreaManager;

    Util::Data::ArrayList<VirtualAddressSpace*> addressSpaces;
    VirtualAddressSpace *currentAddressSpace;
    VirtualAddressSpace &kernelAddressSpace;

    bool serialDebuggingEnabled = false;
    Device::SerialPort debugPort = Device::SerialPort(Device::SerialPort::COM1);
    Util::Stream::OutputStreamWriter debugWriter = Util::Stream::OutputStreamWriter(debugPort);
};

}

#endif