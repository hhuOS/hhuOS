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

#include "lib/libc/printf.h"
#include "kernel/multiboot/Structure.h"
#include "kernel/memory/PageDirectory.h"

#include "kernel/memory/Paging.h"
#include "kernel/core/Management.h"
#include "MemLayout.h"

extern "C" {
    #include "lib/libc/string.h"
    // extern functions from assembler code paging.asm
    void load_page_directory(uint32_t* pdAddress);
    void enable_4KB_paging();
}

// These area should be write-protected because it contains the kernel code
extern uint32_t ___WRITE_PROTECTED_START__[];
extern uint32_t ___WRITE_PROTECTED_END__[];

namespace Kernel {

/**
 * Constructor for Base Page Directory. This directory contains kernel mappings
 * and is built manually for bootstrapping.
 */
PageDirectory::PageDirectory() {
    uint32_t physPagingAreaStart = 0;

    for (uint32_t i = 0; Multiboot::Structure::blockMap[i].blockCount != 0; i++) {
        const auto &block = Multiboot::Structure::blockMap[i];

        if (block.type == Multiboot::Structure::PAGING_RESERVED) {
            physPagingAreaStart = block.startAddress;
            break;
        }
    }

    // calculate end of data and code that is placed by grub/bootloader
    // uint32_t reservedMemoryEnd = Multiboot::Structure::physReservedMemoryEnd;
    // calculate page count containing all memory up to reservedMemoryEnd
    // plus 1024 pages for the initial heap (4mb)
    // uint32_t pageCount = reservedMemoryEnd / PAGESIZE + 1024;

    // table with virtual PT-addresses placed after the PD itself
    virtTableAddresses = (uint32_t *) (VIRT_PAGE_MEM_START + 257 * PAGESIZE);
    // zero memory for PageTables and PageDirectrories
    memset((void *) VIRT_PAGE_MEM_START, 0, PAGESIZE * 1024);
    // base page directory is located at VIRT_PAGE_MEM_START + 1MB,
    // because the first 1MB is used for all Kernel page tables (mapping the kernelspace)
    pageDirectory = (uint32_t *) (VIRT_PAGE_MEM_START + 256 * PAGESIZE);

    // calculate the phys. address of the first pagedirectory (reserve 1 MB for kernel page tables)
    physPageDirectoryAddress = (uint32_t *) (physPagingAreaStart + 256 * PAGESIZE);

    // set up page directory entries for pagetables containing kernel mappings
    // (all pagetables addressing > KERNEL_START are located at the first MB of Paging Area)
    // calculate virtual addresses for these tables
    uint32_t startIdx = KERNEL_START / (PAGESIZE * 1024);
    for (uint32_t i = 0; i < 256; i++) {
        // pointer to the corresponding pagetable (phys. address) - placed right after phys. addresses of the initial heap
        pageDirectory[startIdx + i] = (physPagingAreaStart + i * PAGESIZE) | PAGE_PRESENT | PAGE_READ_WRITE | PAGE_ACCESS_ALL;
        // pointer to corresponding pagetable (virt. address) - placed at the beginning of PagingAreaMemory
        virtTableAddresses[startIdx + i] = VIRT_PAGE_MEM_START + i * PAGESIZE;
    }

    // set the entries for the mapping of reserved memory + initial 4mb-heap
    for (uint32_t i = 0; Multiboot::Structure::blockMap[i].blockCount != 0; i++) {
        const auto &block = Multiboot::Structure::blockMap[i];

        for (uint32_t j = 0; j < block.blockCount * 1024; j++) {
            uint16_t pageDirectoryIndex = GET_PD_IDX((block.virtualStartAddress + j * PAGESIZE));
            uint16_t pageTableIndex = GET_PT_IDX((block.virtualStartAddress + j * PAGESIZE));

            *((uint32_t *) virtTableAddresses[pageDirectoryIndex] + pageTableIndex) = (block.startAddress + j * PAGESIZE) | PAGE_READ_WRITE | PAGE_PRESENT | PAGE_ACCESS_ALL;
        }
    }

    // now, all important mappings in kernelspace (> KERNEL_START) are set up
    // kernelcode + data loaded by grub is placed at KERNEL_START, the initial heap is placed
    // afterwards and the first 4kb pagetables and directories are placed at VIRT_PAGE_MEM_START

    // load the Page Directory into cr3 and enable 4kb-paging via assembler
    load_page_directory(physPageDirectoryAddress);
    enable_4KB_paging();
}

/**
 * Constructor for process Page Directories.
 * 
 * @param basePageDirectory Pointer to the Page Directory with kernel mappgins.
 */
PageDirectory::PageDirectory(PageDirectory *basePageDirectory) {
    // allocate memory for the page directory
    pageDirectory = (uint32_t *) Management::getInstance().allocPageTable();
    // allocate memory to hold the virtual addresses of page tables
    virtTableAddresses = (uint32_t *) Management::getInstance().allocPageTable();
    // catch errors
    if (pageDirectory == nullptr || virtTableAddresses == nullptr) {
        Management::getInstance().freePageTable(pageDirectory);
        Management::getInstance().freePageTable(virtTableAddresses);
        printf("[PAGEDIRECTORY] Error: could not create PageDirectory\n");
        return;

    }
    // get the physical address of the page directory
    physPageDirectoryAddress = (uint32_t *) Management::getInstance().getPhysicalAddress(pageDirectory);
    // get virtual address of the basePageDirectory
    uint32_t *bp_VirtAddress = basePageDirectory->getPageDirectoryVirtualAddress();
    // get pointer to virtual table addresses from basePageDirectory
    uint32_t *bp_VirtTableAddresses = basePageDirectory->getVirtTableAddresses();
    // calculate at which index the kernelspace starts
    uint32_t idx = KERNEL_START / (PAGESIZE * 1024);

    // map the whole kernelspace from the basePageDirectory into the new page directory
    for (; idx < 1024; idx++) {
        pageDirectory[idx] = bp_VirtAddress[idx];
        virtTableAddresses[idx] = bp_VirtTableAddresses[idx];
    }
}

/**
 * Destructor - should never be called in  basePagedirectory
 */
PageDirectory::~PageDirectory() {
    // unmap the compelete user space and free the page frames
    for (uint32_t addr = 0; addr < KERNEL_START; addr += PAGESIZE) {
        this->unmap(addr);
    }
    // free Page Tables corresponding to user space (< 3GB)
    uint32_t idx_max = KERNEL_START / (PAGESIZE * 1024);
    for (uint32_t idx = 0; idx < idx_max; idx++) {
        Management::getInstance().freePageTable((void *) virtTableAddresses[idx]);
    }
    // free PageDirectory itself and list with virtual table addresses
    Management::getInstance().freePageTable((void *) virtTableAddresses);
    Management::getInstance().freePageTable((void *) pageDirectory);

}

void PageDirectory::writeProtectKernelCode() {
    // set the entries for the mapping of first 8 MB
    uint32_t idx = KERNEL_START / (PAGESIZE * 1024);
    // we want to protect parts of kernel code against write access - calculate indeices for this
    uint32_t writeProtectedStart = (((uint32_t) ___WRITE_PROTECTED_START__) - KERNEL_START) / PAGESIZE;
    uint32_t writeProtectedEnd = (((uint32_t) ___WRITE_PROTECTED_END__) - KERNEL_START) / PAGESIZE;

    uint32_t startIndex = writeProtectedStart / PAGESIZE;
    uint32_t endIndex = writeProtectedEnd / PAGESIZE;

    for (uint32_t i = startIndex; i < endIndex; i++) {
        *((uint32_t *) virtTableAddresses[idx] + i) &= ~PAGE_READ_WRITE;
    }
}

/**
 * Maps a virtual address to a given physical address with certain flags.
 */
void PageDirectory::map(uint32_t physAddress, uint32_t virtAddress, uint16_t flags) {

    // use macros to calculate index into page table and directory
    uint32_t pd_idx = GET_PD_IDX(virtAddress);
    uint32_t pt_idx = GET_PT_IDX(virtAddress);

    // if requested page table is not present, create it
    if ((pageDirectory[pd_idx] & PAGE_PRESENT) == 0) {
        Management::getInstance().createPageTable(this, pd_idx);
    }

    // check if the requested page is already mapped -> error
    if ((*((uint32_t *) virtTableAddresses[pd_idx] + pt_idx) & PAGE_PRESENT) != 0) {

        Cpu::throwException(Cpu::Exception::PAGING_ERROR);


        return;
    }
    // create the entry in the corresponding page table
    *((uint32_t *) virtTableAddresses[pd_idx] + pt_idx) = physAddress | flags;

#if DEBUG_PD
    printf("[PAGEDIRECTORY] Mapped virtual address %x to phys address %x\n", (virtAddress & 0xFFFFF000), physAddress);
#endif

}

/**
 * Unmap a given virtual address from this directory.
 */
uint32_t PageDirectory::unmap(uint32_t virtAddress) {
    // get indices into Page Table and Directory
    uint32_t pd_idx = GET_PD_IDX(virtAddress);
    uint32_t pt_idx = GET_PT_IDX(virtAddress);

    // if requested page table is not present, the page cannot be unmapped
    if ((pageDirectory[pd_idx] & PAGE_PRESENT) == 0) {
        return 0;
    }
    // if the page is not mapped, it cannot be unmapped
    if ((*((uint32_t *) virtTableAddresses[pd_idx] + pt_idx) & PAGE_PRESENT) == 0) {
        return 0;
    }

    // calculate corresponding physical Address and set entry to 0
    uint32_t *vTableAddress = (uint32_t *) virtTableAddresses[pd_idx];

    // do not unmap if page is protected
    if (vTableAddress[pt_idx] & PAGE_WRITE_PROTECTED) {
#if DEBUG_PD
        printf("[PAGEDIRECTORY] Unmap not possible - page is protected\n");
#endif
        return 0;
    }

    uint32_t physAddress = (vTableAddress[pt_idx] & 0xFFFFF000);
    vTableAddress[pt_idx] = 0;

    // TODO: unmap PD if pt is empty (counter array?)

    return physAddress;
}

/**
 * Create a new Page Table in this Page Directory
 */
void PageDirectory::createTable(uint32_t idx, uint32_t physAddress, uint32_t virtAddress) {
    // create the directory entry with the physical address of the table
    pageDirectory[idx] = physAddress | PAGE_PRESENT | PAGE_READ_WRITE;
    // keep track of the virtual address of the table
    virtTableAddresses[idx] = virtAddress;
#if DEBUG_PD
    printf("[PAGEDIRECTORY] Created new page table at virtual address %x\n", virtAddress);
#endif
}

/**
 * Get 4kb-aligned physical address corresponding to the given virtual address.
 */
void *PageDirectory::getPhysicalAddress(void *virtAddress) {
    // get indices into Page Table and Directory
    uint32_t pd_idx = GET_PD_IDX((uint32_t) virtAddress);
    uint32_t pt_idx = GET_PT_IDX((uint32_t) virtAddress);

    // if requested page table is not present, the page cannot be mapped
    if ((pageDirectory[pd_idx] & PAGE_PRESENT) == 0) {
        return 0;
    }
    // if the page is not mapped, it cannot be unmapped
    if ((*((uint32_t *) virtTableAddresses[pd_idx] + pt_idx) & PAGE_PRESENT) == 0) {
        return 0;
    }

    // calculate corresponding physical Address and set entry to 0
    uint32_t *vTableAddress = (uint32_t *) virtTableAddresses[pd_idx];
    uint32_t physAddress = (vTableAddress[pt_idx] & 0xFFFFF000);

    return (void *) physAddress;
}

/**
 * Protects a given page from unmapping.
 */
void PageDirectory::protectPage(uint32_t virtAddress) {
    // align 4kb
    uint32_t vaddr = virtAddress & 0xFFFFF000;
    // get indices into Page Table and Directory
    uint32_t pd_idx = GET_PD_IDX(vaddr);
    uint32_t pt_idx = GET_PT_IDX(vaddr);

    // if requested page table is not present, the page cannot be protected
    if ((pageDirectory[pd_idx] & PAGE_PRESENT) == 0) {
#if DEBUG_PD
        printf("[PAGEDIRECTORY] WARN: Page table not present - cannot protect page\n");
#endif
        return;
    }
    // if the page is not mapped, it cannot be protected
    if ((*((uint32_t *) virtTableAddresses[pd_idx] + pt_idx) & PAGE_PRESENT) == 0) {
#if DEBUG_PD
        printf("[PAGEDIRECTORY] WARN: Page not present - cannot protect page\n");
#endif
        return;
    }

    // calculate virtual address of page table
    uint32_t *vTableAddress = (uint32_t *) virtTableAddresses[pd_idx];
    // set protected bit in corresponding entry
    vTableAddress[pt_idx] |= PAGE_WRITE_PROTECTED;
}

/**
 * Protects a range of pages from unmapping.
 */
void PageDirectory::protectPage(uint32_t virtStartAddress, uint32_t virtEndAddress) {
    // align addresses 4kb
    uint32_t startAddr = virtStartAddress & 0xFFFFF000;
    uint32_t endAddr = virtEndAddress & 0xFFFFF000;

    // protect every page
    uint32_t i = 0;
    while (startAddr + i * PAGESIZE < endAddr) {
        protectPage(startAddr + i * PAGESIZE);
        i++;
    }
}

/**
 * Unprotects a given page from unmapping.
 */
void PageDirectory::unprotectPage(uint32_t virtAddress) {
    // align 4kb
    uint32_t vaddr = virtAddress & 0xFFFFF000;
    // get indices into Page Table and Directory
    uint32_t pd_idx = GET_PD_IDX(vaddr);
    uint32_t pt_idx = GET_PT_IDX(vaddr);

    // if requested page table is not present, the page cannot be unprotected
    if ((pageDirectory[pd_idx] & PAGE_PRESENT) == 0) {
#if DEBUG_PD
        printf("[PAGEDIRECTORY] WARN: Page table not present - cannot unprotect page\n");
#endif
        return;
    }
    // if the page is not mapped, it cannot be unprotected
    if ((*((uint32_t *) virtTableAddresses[pd_idx] + pt_idx) & PAGE_PRESENT) == 0) {
#if DEBUG_PD
        printf("[PAGEDIRECTORY] WARN: Page not present - cannot unprotect page\n");
#endif
        return;
    }

    // calculate virtual address of page table
    uint32_t *vTableAddress = (uint32_t *) virtTableAddresses[pd_idx];
    // clean protected bit in corresponding entry
    vTableAddress[pt_idx] &= ~PAGE_WRITE_PROTECTED;
}

/**
 * Unprotects a range of pages from unmapping.
 */
void PageDirectory::unprotectPage(uint32_t virtStartAddress, uint32_t virtEndAddress) {
    // get 4kb aligned addresses
    uint32_t startAddr = virtStartAddress & 0xFFFFF000;
    uint32_t endAddr = virtEndAddress & 0xFFFFF000;

    // unprotect every page
    uint32_t i = 0;
    while (startAddr + i * PAGESIZE < endAddr) {
        unprotectPage(startAddr + i * PAGESIZE);
        i++;
    }
}

}
