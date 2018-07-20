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


