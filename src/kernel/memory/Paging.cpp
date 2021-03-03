/*
 * Copyright (C) 2018-2021 Heinrich-Heine-Universitaet Duesseldorf,
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

#include "kernel/multiboot/Structure.h"
#include "Paging.h"
#include "MemLayout.h"

void Kernel::bootstrapPaging(uint32_t *directory, uint32_t *biosDirectory) {
    // calculate 4MB page where (higher-half) kernel should start
    uint32_t kernelPage = KERNEL_START >> 22U;
    // size of a 4MB page
    uint32_t bigPageSize = PAGESIZE * 1024U;

    // Kernel::Multiboot::Structure::MemoryBlock* &blockMap = *VIRT2PHYS_VAR(Kernel::Multiboot::Structure::MemoryBlock**, Kernel::Multiboot::Structure::blockMap);
    auto* blockMap = (Kernel::Multiboot::Structure::MemoryBlock*) ((uint32_t) (&Kernel::Multiboot::Structure::blockMap) - 0xC0000000);

    uint32_t pageCount = 0;
    for (uint32_t i = 0; blockMap[i].blockCount != 0; i++) {
        uint32_t startAddress = blockMap[i].startAddress;
        blockMap[i].virtualStartAddress = (kernelPage + pageCount) * bigPageSize;

        for (uint32_t j = 0; j < blockMap[i].blockCount; j++) {
            directory[pageCount] = (uint32_t) ((startAddress + j * bigPageSize) | PAGE_PRESENT | PAGE_READ_WRITE | PAGE_SIZE_MiB | PAGE_ACCESS_SUPERVISOR);
            directory[kernelPage + pageCount] = (uint32_t) ((startAddress + j * bigPageSize) | PAGE_PRESENT | PAGE_READ_WRITE | PAGE_SIZE_MiB | PAGE_ACCESS_SUPERVISOR);

            biosDirectory[pageCount] = (uint32_t) ((startAddress + j * bigPageSize) | PAGE_PRESENT | PAGE_READ_WRITE | PAGE_SIZE_MiB | PAGE_ACCESS_SUPERVISOR);
            biosDirectory[kernelPage + pageCount] = (uint32_t) ((startAddress + j * bigPageSize) | PAGE_PRESENT | PAGE_READ_WRITE | PAGE_SIZE_MiB | PAGE_ACCESS_SUPERVISOR);

            pageCount++;
        }
    }

    // Search 2 free block (4 MB each) for initial heap and paging area
    uint32_t blocksFound = 0;
    uint32_t heapPhysicalAddress = 0;
    uint32_t pagingAreaPhysicalAddress = 0;
    for (uint32_t i = 0; blocksFound < 2; i++) {
        if (blockMap[i + 1].startAddress - (blockMap[i].startAddress + blockMap[i].blockCount * bigPageSize) >= bigPageSize || blockMap[i + 1].blockCount == 0) {
            uint32_t physicalAddress = blockMap[i].startAddress + blockMap[i].blockCount * bigPageSize;

            if (blocksFound == 0) {
                heapPhysicalAddress = physicalAddress;
                blockMap[i + 1] = {physicalAddress, (kernelPage + pageCount) * bigPageSize, bigPageSize, 1, Kernel::Multiboot::Structure::HEAP_RESERVED};
            } else {
                pagingAreaPhysicalAddress = physicalAddress;
                blockMap[i + 1] = {physicalAddress, VIRT_PAGE_MEM_START, bigPageSize, 1, Kernel::Multiboot::Structure::PAGING_RESERVED};
            }

            blocksFound++;
        }
    }

    // the first page of the initial heap above the reserved memory is mapped to an offset of KERNEL_START
    // no identity mapping needed because the heap is only used when paging is already enabled
    directory[kernelPage + pageCount] = (uint32_t) (heapPhysicalAddress | PAGE_PRESENT | PAGE_READ_WRITE | PAGE_SIZE_MiB);

    // calculate index to first virtual address of paging area memory
    // these first 4MB of the paging area are needed to set up the final 4KB paging,
    // so map the first (phys.) 4MB after the initial 4MB-heap to this address
    uint32_t pagingAreaIndex = VIRT_PAGE_MEM_START / bigPageSize;
    directory[pagingAreaIndex] = (uint32_t) (pagingAreaPhysicalAddress | PAGE_PRESENT | PAGE_READ_WRITE | PAGE_SIZE_MiB);
}
