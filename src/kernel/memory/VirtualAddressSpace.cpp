/*
 * Copyright (C) 2018-2024 Heinrich-Heine-Universitaet Duesseldorf,
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

#include "VirtualAddressSpace.h"

#include "lib/util/base/Constants.h"
#include "lib/util/base/System.h"
#include "kernel/service/MemoryService.h"
#include "kernel/service/ProcessService.h"
#include "MemoryLayout.h"
#include "kernel/memory/Paging.h"
#include "kernel/process/Process.h"
#include "kernel/service/Service.h"
#include "lib/util/base/Exception.h"
#include "lib/util/base/FreeListMemoryManager.h"
#include "lib/util/collection/ArrayList.h"

namespace Util {

class HeapMemoryManager;

}  // namespace Util

namespace Kernel {

VirtualAddressSpace::VirtualAddressSpace(Paging::Table *physicalPageDirectory, Paging::Table *virtualPageDirectory, Util::HeapMemoryManager &kernelHeapMemoryManager) :
        kernelAddressSpace(true), physicalPageDirectory(physicalPageDirectory), virtualPageDirectory(virtualPageDirectory), memoryManager(kernelHeapMemoryManager) {}

VirtualAddressSpace::VirtualAddressSpace() : kernelAddressSpace(false), physicalPageDirectory(Service::getService<MemoryService>().allocatePageTable()), virtualPageDirectory(new Paging::Table()), memoryManager(Util::System::getAddressSpaceHeader().memoryManager) {
    auto &kernelSpace = Service::getService<ProcessService>().getKernelProcess().getAddressSpace();

    for (uint32_t address = MemoryLayout::KERNEL_AREA.startAddress; address < MemoryLayout::KERNEL_AREA.endAddress; address += 1024 * Util::PAGESIZE) {
        // Get index into page directory
        uint32_t pageDirectoryIndex = Paging::DIRECTORY_INDEX(reinterpret_cast<uint32_t>(address));

        // Copy kernel mapping
        (*physicalPageDirectory)[pageDirectoryIndex] = (*kernelSpace.physicalPageDirectory)[pageDirectoryIndex];
        (*virtualPageDirectory)[pageDirectoryIndex] = (*kernelSpace.virtualPageDirectory)[pageDirectoryIndex];
    }
}

VirtualAddressSpace::~VirtualAddressSpace() {
    if (!kernelAddressSpace) {
        Service::getService<MemoryService>().freePageTable(physicalPageDirectory);
        delete virtualPageDirectory;
    }
}

Util::HeapMemoryManager& VirtualAddressSpace::getMemoryManager() const {
    return memoryManager;
}

bool VirtualAddressSpace::isKernelAddressSpace() const {
    return kernelAddressSpace;
}

void* VirtualAddressSpace::getPhysicalAddress(void *virtualAddress) const {
    // Get indices into page table and directory
    uint32_t pageDirectoryIndex = Paging::DIRECTORY_INDEX(reinterpret_cast<uint32_t>(virtualAddress));
    uint32_t pageTableIndex = Paging::TABLE_INDEX(reinterpret_cast<uint32_t>(virtualAddress));

    // Check if the requested page table is present
    if ((*virtualPageDirectory)[pageDirectoryIndex].isUnused()) {
        return nullptr;
    }

    // Get corresponding page table
    auto &pageTable = *reinterpret_cast<Paging::Table*>((*virtualPageDirectory)[pageDirectoryIndex].getAddress());

    // Check if the requested page is present
    if (pageTable[pageTableIndex].isUnused()) {
        return nullptr;
    }

    // Calculate physical address by reading the frame's start address from the page table and adding the offset
    return reinterpret_cast<void*>(pageTable[pageTableIndex].getAddress() | (reinterpret_cast<uint32_t>(virtualAddress) & 0x00000fff));
}

void VirtualAddressSpace::map(const void *physicalAddress, const void *virtualAddress, uint16_t flags) {
    // Get indices into page table and directory
    uint32_t pageDirectoryIndex = Paging::DIRECTORY_INDEX(reinterpret_cast<uint32_t>(virtualAddress));
    uint32_t pageTableIndex = Paging::TABLE_INDEX(reinterpret_cast<uint32_t>(virtualAddress));
    auto &memoryService = Service::getService<MemoryService>();

    // Check if the requested page table is present and allocate a new one, if necessary
    if ((*virtualPageDirectory)[pageDirectoryIndex].isUnused()) {
        // Allocate a page for the table
        void *virtualPageTable = memoryService.allocatePageTable();
        void *physicalPageTable = getPhysicalAddress(virtualPageTable);

        // Calculate page directory flags
        auto pageDirectoryFlags = Paging::PRESENT | Paging::WRITABLE | (reinterpret_cast<uint32_t>(virtualAddress) >= Kernel::MemoryLayout::KERNEL_AREA.endAddress ? Paging::USER_ACCESSIBLE : Paging::NONE);

        // Check if the virtual address is inside kernel memory.
        // In this case, we need to propagate the mapping to all active address spaces, because the kernel is mapped into each address space.
        if (reinterpret_cast<uint32_t>(virtualAddress) <= MemoryLayout::KERNEL_AREA.endAddress) {
            const auto &addressSpaces = memoryService.getAllAddressSpaces();
            for (uint32_t i = 0; i < addressSpaces.size(); i++) { // Do not use a for-each loop, since the iterator itself requires memory and may cause a deadlock
                auto &addressSpace = *addressSpaces.get(i);
                (*addressSpace.virtualPageDirectory)[pageDirectoryIndex].set(reinterpret_cast<uint32_t>(virtualPageTable), pageDirectoryFlags);
                (*addressSpace.physicalPageDirectory)[pageDirectoryIndex].set(reinterpret_cast<uint32_t>(physicalPageTable), pageDirectoryFlags);
            }
        } else {
            // The virtual address concerns user space memory, and must not be visible to any other address space
            (*virtualPageDirectory)[pageDirectoryIndex].set(reinterpret_cast<uint32_t>(virtualPageTable), pageDirectoryFlags);
            (*physicalPageDirectory)[pageDirectoryIndex].set(reinterpret_cast<uint32_t>(physicalPageTable), pageDirectoryFlags);
        }
    }

    // Get corresponding page table
    auto &pageTable = *reinterpret_cast<Paging::Table*>((*virtualPageDirectory)[pageDirectoryIndex].getAddress());

    // Check if the requested page is already mapped
    if (!pageTable[pageTableIndex].isUnused()) {
        Util::Exception::throwException(Util::Exception::PAGING_ERROR, "PageDirectory: Requested page is already mapped!");
    }

    // Set entry in page table
    pageTable[pageTableIndex].set(reinterpret_cast<uint32_t>(physicalAddress), flags);
}

void* VirtualAddressSpace::unmap(const void *virtualAddress) {
    // Get indices into page table and directory
    uint32_t pageDirectoryIndex = Paging::DIRECTORY_INDEX(reinterpret_cast<uint32_t>(virtualAddress));
    uint32_t pageTableIndex = Paging::TABLE_INDEX(reinterpret_cast<uint32_t>(virtualAddress));

    // Check if the requested page table is present
    if ((*virtualPageDirectory)[pageDirectoryIndex].isUnused()) {
        return nullptr;
    }

    // Get corresponding page table
    auto &pageTable = *reinterpret_cast<Paging::Table*>((*virtualPageDirectory)[pageDirectoryIndex].getAddress());

    // Check if the requested page is present
    if (pageTable[pageTableIndex].isUnused()) {
        return nullptr;
    }

    // Unmap page
    auto physicalAddress = pageTable[pageTableIndex].getAddress();
    pageTable[pageTableIndex].clear();

    // Invalidate entry in TLB
    asm volatile(
            "invlpg (%0)"
            : :
            "r"(virtualAddress)
            );

    // Delete page table, if it is empty
    if (pageTable.isEmpty()) {
        auto &memoryService = Service::getService<MemoryService>();

        // Check if the virtual address is inside kernel memory.
        // In this case, we need to propagate the mapping to all active address spaces, because the kernel is mapped into each address space.
        if (reinterpret_cast<uint32_t>(virtualAddress) <= MemoryLayout::KERNEL_AREA.endAddress) {
            const auto &addressSpaces = memoryService.getAllAddressSpaces();
            for (uint32_t i = 0; i < addressSpaces.size(); i++) { // Do not use a for-each loop, since the iterator itself requires memory and may cause a deadlock
                auto &addressSpace = *addressSpaces.get(i);
                (*addressSpace.virtualPageDirectory)[pageDirectoryIndex].clear();
                (*addressSpace.physicalPageDirectory)[pageDirectoryIndex].clear();
            }
        } else {
            (*virtualPageDirectory)[pageDirectoryIndex].clear();
            (*physicalPageDirectory)[pageDirectoryIndex].clear();
        }

        memoryService.freePageTable(&pageTable);
    }

    return reinterpret_cast<void*>(physicalAddress);
}

const Paging::Table& VirtualAddressSpace::getPageDirectoryPhysical() const {
    return *physicalPageDirectory;
}

}