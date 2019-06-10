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
    // up to this phyiscal address code and data is already placed by grub/bootloader
    // this area should be identity-mapped and the heap should start above
    uint32_t &reservedMemoryEnd = VIRT2PHYS_VAR(uint32_t, Multiboot::Structure::physReservedMemoryEnd);
    // size of a 4mb page
    uint32_t bigPageSize = PAGESIZE * 1024U;
    // use alignment value to align addresses to 4mb
    uint32_t alignment =  bigPageSize - 1U;
    // align end address of reserved memory to 4mb
    reservedMemoryEnd = (reservedMemoryEnd + alignment) & ~alignment;
    // calculate pagecount where reserved memory ends
    uint32_t pageCount = reservedMemoryEnd / bigPageSize;

    // now set up the identity mapping for the code and data up to the end of reserved memory
    // everything should be mapped at its phys. address and at an offset of KERNEL_START
    for (uint32_t i = 0; i < pageCount; i++) {
        directory[i] = (uint32_t) ((i * bigPageSize) | PAGE_PRESENT | PAGE_READ_WRITE | PAGE_SIZE_MiB);
        directory[kernelPage + i] = (uint32_t) ((i * bigPageSize) | PAGE_PRESENT | PAGE_READ_WRITE | PAGE_SIZE_MiB);

        biosDirectory[i] = (uint32_t) ((i * bigPageSize) | PAGE_PRESENT | PAGE_READ_WRITE | PAGE_SIZE_MiB);
        biosDirectory[kernelPage + i] = (uint32_t) ((i * bigPageSize) | PAGE_PRESENT | PAGE_READ_WRITE | PAGE_SIZE_MiB);
    }
    // the first page of the initial heap above the reserved memory is mapped to an offset of KERNEL_START
    // no identity mapping needed because the heap is only used when paging is already enabled
    directory[kernelPage + pageCount] = (uint32_t) ((pageCount * bigPageSize) | PAGE_PRESENT | PAGE_READ_WRITE | PAGE_SIZE_MiB);

    // calculate index to first virtual address of paging area memory
    // these first 4mb of the paging area are needed to set up the final 4kb paging,
    // so map the first (phys.) 4mb after the initial 4mb-heap to this address
    uint32_t pagingAreaIndex = VIRT_PAGE_MEM_START / bigPageSize;
    directory[pagingAreaIndex] = (uint32_t) (((pageCount + 1) * bigPageSize) | PAGE_PRESENT | PAGE_READ_WRITE | PAGE_SIZE_MiB);
}


