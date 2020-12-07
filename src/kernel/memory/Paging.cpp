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

#include <cstdint>
#include "kernel/multiboot/Structure.h"
#include "lib/memory/MemoryUtil.h"
#include "Paging.h"
#include "MemLayout.h"

extern "C" {
    void bootstrapPaging(uint32_t *directory, uint32_t *biosDirectory);
}

/**
 * Function to set up the 4mb Pagedirectories needed for bootstrapping and BIOS-calls.
 * The parameters are assumed to point to physical addresses since paging is not enabled here.
 * In the bootstrap-PD a first initial heap with 4mb and the first 4mb of PagingAreaMemory are mapped
 * because they are needed to bootstrap the final 4kb-paging.
 * Accordingly, until the 4kb paging with pagefault-handling is enabled, the heap should only be used
 * for small allocations so that it does not exceed 4mb.
 *
 * @param directory Pointer to the bootstrapping 4mb-Pagedirectory
 * @param biosDirectory Pointer to the 4mb-Pagedirectory only used for BIOS-calls
 */
void bootstrapPaging(uint32_t *directory, uint32_t *biosDirectory) {
    // calculate 4mb page where (higher-half) kernel should start
    uint32_t kernelPage = KERNEL_START >> 22U;
    // size of a 4mb page
    uint32_t bigPageSize = PAGESIZE * 1024U;

    // Kernel::Multiboot::Structure::MemoryBlock* &blockMap = *VIRT2PHYS_VAR(Kernel::Multiboot::Structure::MemoryBlock**, Kernel::Multiboot::Structure::blockMap);
    auto* blockMap = (Kernel::Multiboot::Structure::MemoryBlock*) ((uint32_t) (&Kernel::Multiboot::Structure::blockMap) - 0xC0000000);

    uint32_t pageCount = 0;
    for (uint32_t i = 0; blockMap[i].blockCount != 0; i++) {
        uint32_t startAddress = blockMap[i].startAddress;

        for (uint32_t j = 0; j < blockMap[i].blockCount; j++) {
            directory[pageCount] = (uint32_t) ((startAddress + j * bigPageSize) | PAGE_PRESENT | PAGE_READ_WRITE | PAGE_SIZE_MiB | PAGE_ACCESS_SUPERVISOR);
            directory[kernelPage + pageCount] = (uint32_t) ((startAddress + j * bigPageSize) | PAGE_PRESENT | PAGE_READ_WRITE | PAGE_SIZE_MiB | PAGE_ACCESS_SUPERVISOR);

            biosDirectory[pageCount] = (uint32_t) ((startAddress + j * bigPageSize) | PAGE_PRESENT | PAGE_READ_WRITE | PAGE_SIZE_MiB | PAGE_ACCESS_SUPERVISOR);
            biosDirectory[kernelPage + pageCount] = (uint32_t) ((startAddress + j * bigPageSize) | PAGE_PRESENT | PAGE_READ_WRITE | PAGE_SIZE_MiB | PAGE_ACCESS_SUPERVISOR);

            pageCount++;
        }
    }

    // Search 8 free physical MB for initial heap and paging area
    uint32_t freePhysicalAddress = 0;
    for (uint32_t i = 0; blockMap[i].blockCount != 0; i++) {
        if (blockMap[i + 1].blockCount == 0) {
            // No 8 MB gap has been found -> We use the memory after the last block
            freePhysicalAddress = blockMap[i].startAddress + blockMap[i].blockCount * bigPageSize;
            blockMap[i + 1] = {freePhysicalAddress, 2 * bigPageSize, 2};
            break;
        }

        if (blockMap[i + 1].startAddress - (blockMap[i].startAddress + blockMap[i].blockCount * bigPageSize) >= 2 * bigPageSize) {
            freePhysicalAddress = blockMap[i].startAddress + blockMap[i].blockCount * bigPageSize;
            blockMap[i + 1] = {freePhysicalAddress, 2 * bigPageSize, 2};
        }
    }

    // the first page of the initial heap above the reserved memory is mapped to an offset of KERNEL_START
    // no identity mapping needed because the heap is only used when paging is already enabled
    directory[kernelPage + pageCount] = (uint32_t) (freePhysicalAddress | PAGE_PRESENT | PAGE_READ_WRITE | PAGE_SIZE_MiB);

    // calculate index to first virtual address of paging area memory
    // these first 4mb of the paging area are needed to set up the final 4kb paging,
    // so map the first (phys.) 4mb after the initial 4mb-heap to this address
    uint32_t pagingAreaIndex = VIRT_PAGE_MEM_START / bigPageSize;
    directory[pagingAreaIndex] = (uint32_t) ((freePhysicalAddress + bigPageSize) | PAGE_PRESENT | PAGE_READ_WRITE | PAGE_SIZE_MiB);
}
