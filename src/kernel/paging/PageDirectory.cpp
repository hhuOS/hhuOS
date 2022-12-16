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

#include "asm_interface.h"
#include "lib/util/memory/Address.h"
#include "kernel/system/System.h"
#include "kernel/multiboot/Multiboot.h"
#include "kernel/paging/PageDirectory.h"
#include "kernel/paging/Paging.h"
#include "MemoryLayout.h"
#include "kernel/service/MemoryService.h"
#include "lib/util/Exception.h"

namespace Kernel {

PageDirectory::PageDirectory() {
    auto *blockMap = Multiboot::getBlockMap();
    uint32_t physPagingAreaStart = 0;

    for (uint32_t i = 0; blockMap[i].blockCount != 0; i++) {
        const auto &block = blockMap[i];

        if (block.type == Multiboot::PAGING_RESERVED) {
            physPagingAreaStart = block.startAddress;
            break;
        }
    }

    // Table with virtual PT-addresses placed after the PD itself
    virtualTableAddresses = reinterpret_cast<uint32_t*>(MemoryLayout::PAGING_AREA.startAddress + 257 * Paging::PAGESIZE);
    // Zero memory for PageTables and PageDirectories
    Util::Memory::Address<uint32_t>(MemoryLayout::PAGING_AREA.startAddress).setRange(0, Paging::PAGESIZE * 258);
    // Base page directory is located at VIRT_PAGE_MEM_START + 1MB, because the first 1MB is used for all Kernel page tables (mapping the kernel)
    pageDirectory = reinterpret_cast<uint32_t*>(MemoryLayout::PAGING_AREA.startAddress + 256 * Paging::PAGESIZE);

    // Calculate the physical address of the first page directory (reserve 1 MiB for kernel page tables)
    pageDirectoryPhysicalAddress = reinterpret_cast<uint32_t*>(physPagingAreaStart + 256 * Paging::PAGESIZE);

    // Set up page directory entries for page tables containing kernel mappings (all pagetables addressing > KERNEL_START are located at the first MB of Paging Area)
    // Calculate virtual addresses for these tables
    uint32_t startIndex = MemoryLayout::KERNEL_START / (Paging::PAGESIZE * 1024);
    for (uint32_t i = 0; i < 256; i++) {
        // Pointer to the corresponding page table (physical address) - placed right after physical addresses of the initial heap
        pageDirectory[startIndex + i] = (physPagingAreaStart + i * Paging::PAGESIZE) | Paging::PRESENT | Paging::READ_WRITE;
        // Pointer to corresponding page table (virtual address) - placed at the beginning of PagingAreaMemory
        virtualTableAddresses[startIndex + i] = MemoryLayout::PAGING_AREA.startAddress + i * Paging::PAGESIZE;
    }

    // set the entries for the mapping of reserved memory + initial 4MB-heap
    for (uint32_t i = 0; blockMap[i].blockCount != 0; i++) {
        const auto &block = blockMap[i];
        if (!block.initialMap) {
            continue;
        }

        // If the block has initialMap set to false, it has not been mapped by the 4MB paging. Thus, it's virtual start address is 0,
        // which leads to virtTableAddresses[0] = 0. This would overwrite the BIOS interrupt vector table and destroy BIOS calls!
        // Blocks with initialMap = false should not been mapped right now, but rather be manually mapped into the kernel heap later on.
        for (uint32_t j = 0; j < block.blockCount * 1024; j++) {
            uint16_t pageDirectoryIndex = Paging::GET_PD_IDX((block.virtualStartAddress + j * Paging::PAGESIZE));
            uint16_t pageTableIndex = Paging::GET_PT_IDX((block.virtualStartAddress + j * Paging::PAGESIZE));

            *(reinterpret_cast<uint32_t*>(virtualTableAddresses[pageDirectoryIndex]) + pageTableIndex) =
                    (block.startAddress + j * Paging::PAGESIZE) | Paging::PRESENT | Paging::READ_WRITE;
        }
    }

    // Now, all important mappings in kernel (> KERNEL_START) are set up
    // Kernel code and data loaded by the bootloader are placed at KERNEL_START, the initial heap is placed
    // afterwards and the first 4 KiB page tables and directories are placed at VIRT_PAGE_MEM_START

    // Load the Page Directory into cr3 and enable 4 KiB paging via assembly code
    load_page_directory(pageDirectoryPhysicalAddress);
    enable_system_paging();
}

PageDirectory::PageDirectory(PageDirectory &basePageDirectory) {
    auto &memoryService = System::getService<Kernel::MemoryService>();
    
    // Allocate memory for the page directory
    pageDirectory = static_cast<uint32_t *>(memoryService.allocatePageTable());
    // Allocate memory to hold the virtual addresses of page tables
    virtualTableAddresses = static_cast<uint32_t *>(memoryService.allocatePageTable());

    if (pageDirectory == nullptr || virtualTableAddresses == nullptr) {
        memoryService.freePageTable(pageDirectory);
        memoryService.freePageTable(virtualTableAddresses);
        Util::Exception::throwException(Util::Exception::OUT_OF_PAGING_MEMORY, "PageDirectory: Failed to allocate page tables!");
    }

    // Get the physical address of the page directory
    pageDirectoryPhysicalAddress = static_cast<uint32_t *>(memoryService.getPhysicalAddress(pageDirectory));
    // Get virtual address of the basePageDirectory
    uint32_t *basePageDirectoryVirtualAddress = basePageDirectory.getPageDirectoryVirtualAddress();
    // Get pointer to virtual table addresses from basePageDirectory
    uint32_t *basePageDirectoryVirtualTableAddress = basePageDirectory.getVirtualTableAddresses();

    // Map the whole kernel from the basePageDirectory into the new page directory
    for (uint32_t index = MemoryLayout::KERNEL_START / (Paging::PAGESIZE * 1024); index < 1024; index++) {
        pageDirectory[index] = basePageDirectoryVirtualAddress[index];
        virtualTableAddresses[index] = basePageDirectoryVirtualTableAddress[index];
    }
}

PageDirectory::~PageDirectory() {
    auto &memoryService = System::getService<Kernel::MemoryService>();

    // Free page tables corresponding to user space (< 3GB)
    uint32_t maxIndex = MemoryLayout::KERNEL_START / (Paging::PAGESIZE * 1024);
    for (uint32_t index = 0; index < maxIndex; index++) {
        memoryService.freePageTable((void *) virtualTableAddresses[index]);
    }

    // Free page directory itself and list with virtual table addresses
    memoryService.freePageTable((void *) virtualTableAddresses);
    memoryService.freePageTable((void *) pageDirectory);
}

void PageDirectory::map(uint32_t physicalAddress, uint32_t virtualAddress, uint16_t flags) {
    auto &memoryService = System::getService<Kernel::MemoryService>();

    // Calculate indices into page table and directory
    uint32_t pageDirectoryIndex = Paging::GET_PD_IDX(virtualAddress);
    uint32_t pageTableIndex = Paging::GET_PT_IDX(virtualAddress);

    // If the requested page table is not present, initialize it
    if ((pageDirectory[pageDirectoryIndex] & Paging::PRESENT) == 0) {
        memoryService.createPageTable(this, pageDirectoryIndex);
    }

    // Check if the requested page is already mapped
    if ((*((uint32_t *) virtualTableAddresses[pageDirectoryIndex] + pageTableIndex) & Paging::PRESENT) != 0) {
        Util::Exception::throwException(Util::Exception::PAGING_ERROR, "PageDirectory: Requested page is already mapped!");
    }

    // Initialize the entry in the corresponding page table
    *((uint32_t *) virtualTableAddresses[pageDirectoryIndex] + pageTableIndex) = physicalAddress | flags;
}

uint32_t PageDirectory::unmap(uint32_t virtualAddress) {
    // Get indices into page table and directory
    uint32_t pageDirectoryIndex = Paging::GET_PD_IDX(virtualAddress);
    uint32_t pageTableIndex = Paging::GET_PT_IDX(virtualAddress);

    // If the requested page table is not present, the page cannot be unmapped
    if ((pageDirectory[pageDirectoryIndex] & Paging::PRESENT) == 0) {
        return 0;
    }

    // If the page is not mapped, it cannot be unmapped
    if ((*((uint32_t *) virtualTableAddresses[pageDirectoryIndex] + pageTableIndex) & Paging::PRESENT) == 0) {
        return 0;
    }

    // Calculate corresponding physical Address and set entry to 0
    auto *vTableAddress = reinterpret_cast<uint32_t *>(virtualTableAddresses[pageDirectoryIndex]);

    // do not unmap if page is protected
    if (vTableAddress[pageTableIndex] & Paging::DO_NOT_UNMAP) {
        return 0;
    }

    uint32_t physAddress = (vTableAddress[pageTableIndex] & 0xFFFFF000);
    vTableAddress[pageTableIndex] = 0;

    // TODO: Unmap page directory if page table is empty (counter array?)
    return physAddress;
}

void PageDirectory::createTable(uint32_t index, uint32_t physicalAddress, uint32_t virtualAddress, uint32_t flags) {
    // Initialize the directory entry with the physical address of the table
    pageDirectory[index] = physicalAddress | flags;
    // Keep track of the virtual address of the table
    virtualTableAddresses[index] = virtualAddress;
}

void *PageDirectory::getPhysicalAddress(void *virtualAddress) {
    // Get indices into page table and directory
    uint32_t pageDirectoryIndex = Paging::GET_PD_IDX((uint32_t) virtualAddress);
    uint32_t pageTableIndex = Paging::GET_PT_IDX((uint32_t) virtualAddress);

    // Check if the requested page table is present
    if ((pageDirectory[pageDirectoryIndex] & Paging::PRESENT) == 0) {
        return nullptr;
    }

    // Check if the requested page is present
    if ((*((uint32_t *) virtualTableAddresses[pageDirectoryIndex] + pageTableIndex) & Paging::PRESENT) == 0) {
        return nullptr;
    }

    // Calculate corresponding physical Address and set entry to 0
    auto *vTableAddress = reinterpret_cast<uint32_t *>(virtualTableAddresses[pageDirectoryIndex]);
    auto physAddress = vTableAddress[pageTableIndex] & 0xFFFFF000;

    return reinterpret_cast<void*>(physAddress);
}

void PageDirectory::setPageFlags(uint32_t virtualStartAddress, uint32_t flags) {
    // Align address to 4 KiB
    uint32_t alignedAddress = virtualStartAddress & 0xFFFFF000;

    // Get indices into page table and directory
    uint32_t pageDirectoryIndex = Paging::GET_PD_IDX(alignedAddress);
    uint32_t pageTableIndex = Paging::GET_PT_IDX(alignedAddress);

    // If the requested page table is not present, the page cannot be protected
    if ((pageDirectory[pageDirectoryIndex] & Paging::PRESENT) == 0) {
        Util::Exception::throwException(Util::Exception::ILLEGAL_STATE, "PageDirectory: Requested page table is not present!");
    }
    // if the page is not mapped, it cannot be protected
    if ((*((uint32_t *) virtualTableAddresses[pageDirectoryIndex] + pageTableIndex) & Paging::PRESENT) == 0) {
        Util::Exception::throwException(Util::Exception::ILLEGAL_STATE, "PageDirectory: Trying to protect an unmapped page!");
    }

    // Calculate virtual address of page table
    auto *vTableAddress = reinterpret_cast<uint32_t*>(virtualTableAddresses[pageDirectoryIndex]);
    // Set protected bit in corresponding entry
    vTableAddress[pageTableIndex] |= flags;
}

void PageDirectory::setPageFlags(uint32_t virtualStartAddress, uint32_t virtualEndAddress, uint32_t flags) {
    // Align addresses to 4 KiB
    uint32_t alignedStartAddress = virtualStartAddress & 0xFFFFF000;
    uint32_t alignedEndAddress = virtualEndAddress & 0xFFFFF000;

    // Protect each page
    for (uint32_t i = 0; alignedStartAddress + i * Paging::PAGESIZE < alignedEndAddress; i++) {
        setPageFlags(alignedStartAddress + i * Paging::PAGESIZE, flags);
    }
}

void PageDirectory::unsetPageFlags(uint32_t virtualAddress, uint32_t flags) {
    // Align address to 4 KiB
    uint32_t vaddr = virtualAddress & 0xFFFFF000;

    // Get indices into page table and directory
    uint32_t pageDirectoryIndex = Paging::GET_PD_IDX(vaddr);
    uint32_t pageTableIndex = Paging::GET_PT_IDX(vaddr);

    // If the requested page table is not present, the page cannot be unprotected
    if ((pageDirectory[pageDirectoryIndex] & Paging::PRESENT) == 0) {
        Util::Exception::throwException(Util::Exception::ILLEGAL_STATE, "PageDirectory: Requested page table is not present!");
    }
    // If the page is not mapped, it cannot be unprotected
    if ((*((uint32_t *) virtualTableAddresses[pageDirectoryIndex] + pageTableIndex) & Paging::PRESENT) == 0) {
        Util::Exception::throwException(Util::Exception::ILLEGAL_STATE, "PageDirectory: Trying to unprotect an unmapped page!");
    }

    // Calculate virtual address of page table
    auto *vTableAddress = reinterpret_cast<uint32_t *>(virtualTableAddresses[pageDirectoryIndex]);
    // Clear protected bit in corresponding entry
    vTableAddress[pageTableIndex] &= ~flags;
}

void PageDirectory::unsetPageFlags(uint32_t virtualStartAddress, uint32_t virtualEndAddress, uint32_t flags) {
    // Align addresses to 4 KiB
    uint32_t alignedStartAddress = virtualStartAddress & 0xFFFFF000;
    uint32_t alignedEndAddress = virtualEndAddress & 0xFFFFF000;

    // Unprotect each page
    for (uint32_t i = 0; alignedStartAddress + i * Paging::PAGESIZE < alignedEndAddress; i++) {
        unsetPageFlags(alignedStartAddress + i * Paging::PAGESIZE, flags);
    }
}

}
