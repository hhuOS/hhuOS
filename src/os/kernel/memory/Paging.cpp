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
#include <lib/multiboot/Structure.h>
#include <lib/memory/MemoryUtil.h>
#include "Paging.h"
#include "MemLayout.h"

extern "C" {
    void bootstrapPaging(uint32_t *directory, uint32_t *biosDirectory);
}

void bootstrapPaging(uint32_t *directory, uint32_t *biosDirectory) {

    uint32_t kernelPage = KERNEL_START >> 22U;

    uint32_t &reservedMemoryEnd = VIRT2PHYS_VAR(uint32_t, Multiboot::Structure::physReservedMemoryEnd);

    uint32_t bigPageSize = PAGESIZE * 1024U;

    uint32_t alignment =  bigPageSize - 1U;

    reservedMemoryEnd = (reservedMemoryEnd + alignment) & ~alignment;

    uint32_t pageCount = reservedMemoryEnd / bigPageSize;

    for (uint32_t i = 0; i < pageCount; i++) {

        directory[i] = (uint32_t) ((i * bigPageSize) | PAGE_PRESENT | PAGE_READ_WRITE | PAGE_SIZE_MiB);

        directory[kernelPage + i] = (uint32_t) ((i * bigPageSize) | PAGE_PRESENT | PAGE_READ_WRITE | PAGE_SIZE_MiB);

        biosDirectory[i] = (uint32_t) ((i * bigPageSize) | PAGE_PRESENT | PAGE_READ_WRITE | PAGE_SIZE_MiB);

        biosDirectory[kernelPage + i] = (uint32_t) ((i * bigPageSize) | PAGE_PRESENT | PAGE_READ_WRITE | PAGE_SIZE_MiB);
    }

    directory[kernelPage + pageCount] = (uint32_t) ((pageCount * bigPageSize) | PAGE_PRESENT | PAGE_READ_WRITE | PAGE_SIZE_MiB);

    uint32_t pagingAreaIndex = VIRT_PAGE_MEM_START / bigPageSize;

    directory[pagingAreaIndex] = (uint32_t) (((pageCount + 1) * bigPageSize) | PAGE_PRESENT | PAGE_READ_WRITE | PAGE_SIZE_MiB);
}


