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

#include <stdarg.h>

#include "kernel/memory/Paging.h"
#include "kernel/service/InterruptService.h"
#include "kernel/memory/MemoryLayout.h"
#include "MemoryService.h"
#include "kernel/service/MemoryService.h"
#include "kernel/memory/PageFrameAllocator.h"
#include "kernel/memory/PagingAreaManager.h"
#include "kernel/memory/VirtualAddressSpace.h"
#include "lib/util/base/Panic.h"
#include "lib/util/base/HeapMemoryManager.h"
#include "lib/util/base/System.h"
#include "device/cpu/Cpu.h"
#include "kernel/service/Service.h"
#include "lib/util/base/Address.h"
#include "lib/util/base/Constants.h"
#include "device/system/Bios.h"

namespace Kernel {

MemoryService::MemoryService(PageFrameAllocator *pageFrameAllocator, PagingAreaManager *pagingAreaManager, VirtualAddressSpace *kernelAddressSpace) :
        pageFrameAllocator(*pageFrameAllocator), pagingAreaManager(*pagingAreaManager), pageFrameSlabAllocator(reinterpret_cast<uint8_t*>(allocatePhysicalMemory(SlabAllocator::MAX_SLAB_SIZE / Util::PAGESIZE))),
        kernelStackAllocator(reinterpret_cast<uint8_t*>(MemoryLayout::KERNEL_STACK_AREA.startAddress), reinterpret_cast<uint8_t*>(MemoryLayout::KERNEL_STACK_AREA.endAddress), MemoryLayout::KERNEL_STACK_SIZE),
        currentAddressSpace(kernelAddressSpace), kernelAddressSpace(*kernelAddressSpace) {
    addressSpaces.add(kernelAddressSpace);

    Service::getService<InterruptService>().assignSystemCall(Util::System::UNMAP, [](uint32_t paramCount, va_list arguments) -> bool {
        if (paramCount < 2) {
            return false;
        }

        auto &memoryService = Kernel::Service::getService<Kernel::MemoryService>();
        auto virtualAddress = va_arg(arguments, void*);
        auto pageCount = va_arg(arguments, uint32_t);
        auto breakCount = va_arg(arguments, uint32_t);

        if (reinterpret_cast<uint32_t>(virtualAddress) < MemoryLayout::KERNEL_END) {
            return false;
        }

        return memoryService.unmap(virtualAddress, pageCount, breakCount) != nullptr;
    });

    Service::getService<InterruptService>().assignSystemCall(Util::System::MAP_IO, [](uint32_t paramCount, va_list arguments) -> bool {
        if (paramCount < 3) {
            return false;
        }

        auto &memoryService = Kernel::Service::getService<Kernel::MemoryService>();
        auto *physicalAddress = va_arg(arguments, void*);
        auto pageCount = va_arg(arguments, uint32_t);
        void *&mappedAddress = *va_arg(arguments, void**);

        mappedAddress = memoryService.mapIO(physicalAddress, pageCount, false);
        return true;
    });
}

MemoryService::~MemoryService() {
    delete &pageFrameAllocator;
    delete &pagingAreaManager;

    for (const auto *addressSpace : addressSpaces) {
        delete addressSpace;
    }
}

void *MemoryService::allocateKernelMemory(uint32_t size, uint32_t alignment) {
    return kernelAddressSpace.getMemoryManager().allocateMemory(size, alignment);
}

void *MemoryService::reallocateKernelMemory(void *pointer, uint32_t size, uint32_t alignment) {
    return kernelAddressSpace.getMemoryManager().reallocateMemory(pointer, size, alignment);
}

void MemoryService::freeKernelMemory(void *pointer, uint32_t alignment) {
    kernelAddressSpace.getMemoryManager().freeMemory(pointer, alignment);
}

void *MemoryService::allocateUserMemory(uint32_t size, uint32_t alignment) {
    return currentAddressSpace->getMemoryManager().allocateMemory(size, alignment);
}

void *MemoryService::reallocateUserMemory(void *pointer, uint32_t size, uint32_t alignment) {
    return currentAddressSpace->getMemoryManager().reallocateMemory(pointer, size, alignment);
}

void MemoryService::freeUserMemory(void *pointer, uint32_t alignment) {
    currentAddressSpace->getMemoryManager().freeMemory(pointer, alignment);
}

void* MemoryService::allocateBiosMemory(uint32_t pageCount) {
    // Allocate memory below 1 MiB
    void *physicalAddress = allocatePhysicalMemory(pageCount, nullptr);
    if (reinterpret_cast<uint32_t>(physicalAddress) >= Device::Bios::MAX_USABLE_ADDRESS) {
        freePhysicalMemory(physicalAddress, pageCount);
        return nullptr;
    }

    // Allocate page aligned virtual memory
    auto &manager = kernelAddressSpace.getMemoryManager();
    void *virtualAddress = manager.allocateMemory(pageCount * Util::PAGESIZE, Util::PAGESIZE);

    // Create mapping
    uint32_t flags = Paging::PRESENT | Paging::WRITABLE;

    for (uint32_t i = 0; i < pageCount; i++) {
        void *currentPhysicalAddress = reinterpret_cast<uint8_t*>(physicalAddress) + i * Util::PAGESIZE;
        void *currentVirtualAddress = reinterpret_cast<uint8_t*>(virtualAddress) + i * Util::PAGESIZE;

        // If the virtual address is already mapped, we have to unmap it.
        // This can happen because the headers of the free list are mapped to arbitrary physical addresses, but the memory should be mapped to the given physical addresses.
        unmap(currentVirtualAddress, 1);
        // Map the page into the current address space
        currentAddressSpace->map(currentPhysicalAddress, currentVirtualAddress, flags);
    }

    return virtualAddress;
}

void* MemoryService::allocateIsaMemory(uint32_t pageCount) {
    // Allocate memory below 16 MiB
    void *physicalAddress = allocatePhysicalMemory(pageCount, reinterpret_cast<void*>(MemoryLayout::KERNEL_START));
    if (reinterpret_cast<uint32_t>(physicalAddress) >= Device::Isa::MAX_DMA_ADDRESS) {
        freePhysicalMemory(physicalAddress, pageCount);
        return nullptr;
    }

    // Allocate page aligned virtual memory
    auto &manager = kernelAddressSpace.getMemoryManager();
    void *virtualAddress = manager.allocateMemory(pageCount * Util::PAGESIZE, Util::PAGESIZE);

    // Create mapping
    uint32_t flags = Paging::PRESENT | Paging::WRITABLE | Paging::CACHE_DISABLE;

    for (uint32_t i = 0; i < pageCount; i++) {
        void *currentPhysicalAddress = reinterpret_cast<uint8_t*>(physicalAddress) + i * Util::PAGESIZE;
        void *currentVirtualAddress = reinterpret_cast<uint8_t*>(virtualAddress) + i * Util::PAGESIZE;

        // If the virtual address is already mapped, we have to unmap it.
        // This can happen because the headers of the free list are mapped to arbitrary physical addresses, but the memory should be mapped to the given physical addresses.
        unmap(currentVirtualAddress, 1);
        // Map the page into the current address space
        currentAddressSpace->map(currentPhysicalAddress, currentVirtualAddress, flags);
    }

    return virtualAddress;
}

void* MemoryService::allocatePhysicalMemory(uint32_t frameCount, void *startAddress) {
    if (slabAllocatorEnabled && reinterpret_cast<uint32_t>(startAddress) >= Device::Isa::MAX_DMA_ADDRESS) {
        void *physicalStartAddress = pageFrameSlabAllocator.allocateBlock(frameCount);
        if (physicalStartAddress != nullptr) {
            return physicalStartAddress;
        }
    }

    void *physicalStartAddress = pageFrameAllocator.allocateBlockAfterAddress(startAddress);
    void *currentPhysicalAddress = physicalStartAddress;
    void *lastPhysicalAddress;
    bool contiguous;

    // Page frame allocator only works with independent physical frames (4 KiB).
    // We allocate a large chunk of physical memory, by allocating single frames and checking, if they are contiguous
    do {
        contiguous = true;

        for (uint32_t i = 1; i < frameCount; i++) {
            lastPhysicalAddress = currentPhysicalAddress;
            currentPhysicalAddress = pageFrameAllocator.allocateBlockAfterAddress(physicalStartAddress);

            if (reinterpret_cast<uint32_t>(currentPhysicalAddress) - reinterpret_cast<uint32_t>(lastPhysicalAddress) != Util::PAGESIZE) {
                contiguous = false;

                // Allocated frames are not contiguous -> Free them and try again
                freePhysicalMemory(physicalStartAddress, i);

                physicalStartAddress = currentPhysicalAddress;
                break;
            }
        }
    } while (!contiguous);

    return physicalStartAddress;
}

void MemoryService::freePhysicalMemory(void *pointer, uint32_t frameCount) {
    if (slabAllocatorEnabled && pageFrameSlabAllocator.freeBlock(pointer)) {
        return;
    }

    for (uint32_t i = 0; i < frameCount; i++) {
        pageFrameAllocator.freeBlock(static_cast<uint8_t*>(pointer) + i * Util::PAGESIZE);
    }
}

void* MemoryService::allocateKernelStack() {
    return kernelStackAllocator.allocateBlock();
}

void MemoryService::freeKernelStack([[maybe_unused]] void *pointer) {
    unmap(pointer, MemoryLayout::KERNEL_STACK_SIZE / Util::PAGESIZE, 0);
    kernelStackAllocator.freeBlock(pointer);
}

Paging::Table* MemoryService::allocatePageTable() {
    auto *pageTable = static_cast<Paging::Table*>(pagingAreaManager.allocateBlock());
    pageTable->clear();

    return pageTable;
}

void MemoryService::freePageTable(Paging::Table *pageTable) {
    void *physicalAddress = currentAddressSpace->unmap(pageTable);
    if (physicalAddress == nullptr) {
        return;
    }

    // Free virtual memory
    pagingAreaManager.freeBlock(pageTable);
}

void Kernel::MemoryService::map(void *virtualAddress, uint32_t pageCount, uint16_t flags, bool abortIfLocked) {
    for (uint32_t i = 0; i < pageCount; i++) {
        // Allocate a physical page frames to where the page should be mapped
        auto *physicalAddress = pageFrameAllocator.allocateBlock();
        // Map the frame to given virtual address
        currentAddressSpace->map(physicalAddress, reinterpret_cast<uint8_t*>(virtualAddress) + i * Util::PAGESIZE, flags, abortIfLocked);
    }
}

void* Kernel::MemoryService::unmap(void *virtualAddress, uint32_t pageCount, uint32_t breakCount) {
    // Check if the virtual address is page aligned
    if (reinterpret_cast<uint32_t>(virtualAddress) % Util::PAGESIZE != 0) {
        Util::Panic::fire(Util::Panic::INVALID_ARGUMENT, "MemoryService: unmap() called with unaligned virtual address!");
    }

    // Loop through pages and unmap them individually
    void *physicalAddress = nullptr;
    uint8_t nonMappedCount = 0;
    for (uint32_t i = 0; i < pageCount; i++) {
        auto currentVirtualAddress = reinterpret_cast<uint32_t>(virtualAddress) + (i * Util::PAGESIZE);
        physicalAddress = currentAddressSpace->unmap(reinterpret_cast<const void*>(currentVirtualAddress));

        if (physicalAddress == nullptr) {
            nonMappedCount++;
        } else {
            nonMappedCount = 0;
            freePhysicalMemory(physicalAddress, 1);
        }

        // TODO: This is ugly! We need a proper management for mapped/unmapped pages
        // If there were eight pages after each other already unmapped, we break here.
        // This is sort of a workaround because by merging large free memory blocks in memory management
        // it might happen that some parts of the memory are already unmapped.
        if (breakCount > 0 && nonMappedCount == breakCount) {
            break;
        }
    }

    return physicalAddress;
}

void Kernel::MemoryService::mapPhysical(void *physicalAddress, void *virtualAddress, uint32_t pageCount, uint16_t flags) {
    for (uint32_t i = 0; i < pageCount; i++) {
        void *currentPhysicalAddress = reinterpret_cast<uint8_t*>(physicalAddress) + i * Util::PAGESIZE;
        void *currentVirtualAddress = reinterpret_cast<uint8_t*>(virtualAddress) + i * Util::PAGESIZE;

        // If the virtual address is already mapped, we have to unmap it.
        // This can happen because the headers of the free list are mapped to arbitrary physical addresses, but the memory should be mapped to the given physical addresses.
        unmap(currentVirtualAddress, 1);
        // Mark the physical page frame as used
        currentPhysicalAddress = pageFrameAllocator.allocateBlockAtAddress(currentPhysicalAddress);
        // Map the page into the current address space
        currentAddressSpace->map(currentPhysicalAddress, currentVirtualAddress, flags);
    }
}

void *MemoryService::mapIO(uint32_t pageCount, bool mapToKernelHeap) {
    // Allocate block of physical memory
    void *physicalAddress = allocatePhysicalMemory(pageCount);
    // Allocate a block of page aligned virtual memory in the heap
    auto &manager = mapToKernelHeap ? kernelAddressSpace.getMemoryManager() : currentAddressSpace->getMemoryManager();
    void *virtualAddress = manager.allocateMemory(pageCount * Util::PAGESIZE, Util::PAGESIZE);

    // Create mapping
    uint32_t flags = Paging::PRESENT | Paging::WRITABLE | Paging::CACHE_DISABLE | (reinterpret_cast<uint32_t>(virtualAddress) >= Kernel::MemoryLayout::KERNEL_END ? Paging::USER_ACCESSIBLE : Paging::NONE);
    for (uint32_t i = 0; i < pageCount; i++) {
        void *currentPhysicalAddress = reinterpret_cast<uint8_t*>(physicalAddress) + i * Util::PAGESIZE;
        void *currentVirtualAddress = reinterpret_cast<uint8_t*>(virtualAddress) + i * Util::PAGESIZE;

        // If the virtual address is already mapped, we have to unmap it.
        // This can happen because the headers of the free list are mapped to arbitrary physical addresses, but the memory should be mapped to the given physical addresses.
        unmap(currentVirtualAddress, 1);
        // Map the page into the current address space
        currentAddressSpace->map(currentPhysicalAddress, currentVirtualAddress, flags);
    }

    return virtualAddress;
}

void *Kernel::MemoryService::mapIO(void *physicalAddress, uint32_t pageCount, bool mapToKernelHeap) {
    // Allocate page aligned virtual memory
    auto &manager = mapToKernelHeap ? kernelAddressSpace.getMemoryManager() : currentAddressSpace->getMemoryManager();
    void *virtualAddress = manager.allocateMemory(pageCount * Util::PAGESIZE, Util::PAGESIZE);

    // Create mapping
    uint32_t flags = Paging::PRESENT | Paging::WRITABLE | Paging::CACHE_DISABLE | (reinterpret_cast<uint32_t>(virtualAddress) >= Kernel::MemoryLayout::KERNEL_END ? Paging::USER_ACCESSIBLE : Paging::NONE);
    for (uint32_t i = 0; i < pageCount; i++) {
        void *currentPhysicalAddress = reinterpret_cast<uint8_t*>(physicalAddress) + i * Util::PAGESIZE;
        void *currentVirtualAddress = reinterpret_cast<uint8_t*>(virtualAddress) + i * Util::PAGESIZE;

        // If the virtual address is already mapped, we have to unmap it.
        // This can happen because the headers of the free list are mapped to arbitrary physical addresses, but the memory should be mapped to the given physical addresses.
        unmap(currentVirtualAddress, 1);
        // Mark the physical page frame as used
        currentPhysicalAddress = pageFrameAllocator.allocateBlockAtAddress(currentPhysicalAddress);
        // Map the page into the current address space
        currentAddressSpace->map(currentPhysicalAddress, currentVirtualAddress, flags);
    }

    return virtualAddress;
}

void* MemoryService::getPhysicalAddress(void *virtualAddress) {
    return currentAddressSpace->getPhysicalAddress(virtualAddress);
}

VirtualAddressSpace& MemoryService::createAddressSpace() {
    auto addressSpace = new VirtualAddressSpace();
    addressSpaces.add(addressSpace);

    return *addressSpace;
}

void MemoryService::switchAddressSpace(VirtualAddressSpace &addressSpace) {
    if (currentAddressSpace == &addressSpace) {
        return;
    }

    // Get physical address of new page directory
    auto *pageDirectoryPhysical = getPhysicalAddress(const_cast<void*>(reinterpret_cast<const void*>(&addressSpace.getPageDirectoryPhysical())));

    // Set current address space
    currentAddressSpace = &addressSpace;

    asm volatile (
            "mov %0, %%cr3"
            : :
            "r"(pageDirectoryPhysical)
            );
}

void MemoryService::removeAddressSpace(VirtualAddressSpace &addressSpace) {
    if (currentAddressSpace == &addressSpace) {
        Util::Panic::fire(Util::Panic::INVALID_ARGUMENT, "MemoryService: Trying to delete the currently active address space!");
    }

    addressSpaces.remove(&addressSpace);
    delete &addressSpace;
}

void MemoryService::handlePageFault(uint32_t errorCode) {
    // The faulted linear address is stored in the cr2 register
    const auto faultAddress = Device::Cpu::readCr2();

    // Check if page fault was caused by an illegal page access
    if ((errorCode & 0x00000001u) > 0) {
        // Check for null pointer access in user space application
        if (faultAddress == 0) {
            Util::Panic::fire(Util::Panic::NULL_POINTER, "Page fault at address 0x00000000!");
        }

        Util::Panic::fire(Util::Panic::PAGING_ERROR, "Privilege level not sufficient to access page!");
    }

    // Page fault was caused by a non-present page
    const auto alignedFaultAddress = Util::Address(faultAddress).alignDown(Util::PAGESIZE).get();

    // Check if the faulted address is inside a user space stack
    // If the fault occurs right at the start of a user space stack, a stack overflow happened
    if (faultAddress >= Util::USER_SPACE_STACK_MEMORY_START_ADDRESS && alignedFaultAddress % Util::MAX_USER_STACK_SIZE == 0) {
        Util::Panic::fire(Util::Panic::STACK_OVERFLOW, "Page fault below user space stack!");
    }

    // Map the faulted Page
    const auto userSpace = faultAddress >= MemoryLayout::KERNEL_AREA.endAddress;
    map(reinterpret_cast<void*>(faultAddress), 1, Paging::PRESENT | Paging::WRITABLE | (userSpace ? Paging::USER_ACCESSIBLE : Paging::NONE), true);
}

MemoryService::MemoryStatus MemoryService::getMemoryStatus() {
    return {pageFrameAllocator.getTotalMemory(), pageFrameAllocator.getFreeMemory(),
            kernelAddressSpace.getMemoryManager().getTotalMemory(), kernelAddressSpace.getMemoryManager().getFreeMemory(),
            pagingAreaManager.getTotalMemory(), pagingAreaManager.getFreeMemory()};
}

VirtualAddressSpace& MemoryService::getKernelAddressSpace() const {
    return kernelAddressSpace;
}

VirtualAddressSpace &MemoryService::getCurrentAddressSpace() const {
    return *currentAddressSpace;
}

const Util::ArrayList<VirtualAddressSpace *> &MemoryService::getAllAddressSpaces() const {
    return addressSpaces;
}

void MemoryService::enableSlabAllocator() {
    slabAllocatorEnabled = true;
}

}