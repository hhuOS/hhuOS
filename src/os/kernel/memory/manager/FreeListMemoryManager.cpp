/* List-based memory manager for the kernel heap
 *
 * @author Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schoettner
 * @date HHU, 2018
 */

#include <lib/libc/printf.h>
#include <lib/memory/MemoryUtil.h>
#include <kernel/memory/Paging.h>
#include "FreeListMemoryManager.h"

#include "kernel/memory/SystemManagement.h"
#include "kernel/memory/MemLayout.h"

/**
 * Constructor -calls constructor of base class.
 */
FreeListMemoryManager::FreeListMemoryManager(uint32_t memoryEndAddress, uint32_t memoryStartAddress, bool doUnmap)
        : MemoryManager(
        memoryStartAddress, memoryEndAddress, doUnmap) {
    if(freeMemory < sizeof(FLHeader)) {
        // Available Kernel-Memory is too small for a Chunk
        firstChunk = nullptr;
    }else {
        // set up first Chunk of memory
        firstChunk = (FLHeader*) memoryStartAddress;
        freeMemory -= sizeof(FLHeader);
        firstChunk->size = freeMemory;
        firstChunk->next = nullptr;
        firstChunk->prev = nullptr;
    }
}

/**
 * Allocate memory block with given size.
 */
void* FreeListMemoryManager::alloc(uint32_t size) {

    return alloc(size, 0);
}

/**
 * Frees a given memory block
 */
void FreeListMemoryManager::free(void* ptr) {

    if(ptr == nullptr) {
        return;
    }

    if ((uint32_t) ptr < memoryStartAddress || (uint32_t) ptr > memoryEndAddress) {
        return;
    }

    lock.acquire();

    auto * header = (FLHeader*)((uint8_t*) ptr - HEADER_SIZE);

    // Place free block at the right position
    if (firstChunk == nullptr) {
        firstChunk = header;
    } else if(header < firstChunk){
        header->next = firstChunk;
        header->prev = nullptr;
        firstChunk->prev = header;
        firstChunk = header;
    } else {
        FLHeader* tmp = firstChunk;
        while(tmp != nullptr){

            if(header > tmp && (header < tmp->next || tmp->next == nullptr)){
                header->next = tmp->next;
                header->prev = tmp;

                if (header->next != nullptr) {
                    header->next->prev = header;
                }

                header->prev->next = header;
                break;
            }

            tmp = tmp->next;
        }
    }

    FLHeader *mergedHeader = merge(header);

    // if the free chunk has more than 4kb of memory, a page can possibly be unmapped
    if(mergedHeader->size >= PAGESIZE && SystemManagement::isInitialized()) {
        uint32_t addr = (uint32_t) mergedHeader;
        uint32_t chunkEndAddr = addr + (HEADER_SIZE + mergedHeader->size);

        // try to unmap the free memory, not the list header!
        SystemManagement::getInstance()->unmap(addr + HEADER_SIZE, chunkEndAddr - 1);
    }

    lock.release();
}

/**
 * Dump memory list
 */
void FreeListMemoryManager::dump() {
    printf("  Free memory dump\n");
    printf("  ================\n");

    FLHeader* tmp = firstChunk;

    while ( tmp != 0 ) {
        printf("    Start: %x" , (FLHeader*)((uint8_t*)tmp + sizeof(FLHeader)));
        printf(", End: %x" , (FLHeader*)((uint8_t*)tmp + sizeof(FLHeader) + tmp->size));
        printf("  Next: %x", tmp->next);
        printf(", Size: %d\n" , tmp->size);

        tmp = tmp->next;
    }

    printf("\n");
}

FreeListMemoryManager::FLHeader* FreeListMemoryManager::findNext(FLHeader *start, uint32_t reqSize) {
    FLHeader *current = start;

    while (current != nullptr) {
        if (current->size >= reqSize) {
            return current;
        }

        current = current->next;
    }

    return current;
}



void *FreeListMemoryManager::alloc(uint32_t size, uint32_t alignment) {

    if (size == 0) {
        return nullptr;
    }

    size = MemoryUtil::alignUp(size, sizeof(uint32_t));

    lock.acquire();

    FLHeader *current = firstChunk;
    FLHeader *aligned = nullptr;

    while(current != nullptr){

        if (current->size >= size) {

            uint32_t data = ((uint32_t) current) + HEADER_SIZE;
            uint32_t alignedData = MemoryUtil::alignUp(data, alignment);

            // Found free Memory Block with required alignment
            if (data == alignedData) {
                break;
            }

            // Make sure the aligned Memory Block's header
            // does not overlap with the current one
            aligned = (FLHeader*) (MemoryUtil::alignUp(data + HEADER_SIZE + MIN_BLOCK_SIZE, alignment) - HEADER_SIZE);

            // Check if current block has enough free
            // data space to fit in the aligned block
            if (((uint32_t) aligned) + HEADER_SIZE + size <= ((uint32_t) current) + HEADER_SIZE + current->size) {

                aligned->size = ((uint32_t) current) + HEADER_SIZE + current->size - ((uint32_t) aligned) + HEADER_SIZE;
                current->size = (uint32_t) aligned - ((uint32_t) current) + HEADER_SIZE;

                aligned->prev = current;
                aligned->next = current->next;

                if (aligned->next != nullptr) {
                    aligned->next->prev = aligned;
                }

                aligned->prev->next = aligned;

                current = aligned;

                break;
            }
        }

        current = findNext(current->next, size);
    }

    // No memory left
    if (current == nullptr) {
        return nullptr;
    }

    // Check if the choosen chunk can be sliced in two parts
    if (current->size - size >= MIN_BLOCK_SIZE + HEADER_SIZE) {
        FLHeader* slice = (FLHeader*) (((uint32_t)current) + HEADER_SIZE + size);

        slice->size = current->size - size - HEADER_SIZE;
        slice->next = current->next;
        slice->prev = current->prev;

        if (slice->next != nullptr) {
            slice->next->prev = slice;
        }

        if (slice->prev != nullptr) {
            slice->prev->next = slice;
        } else {
            firstChunk = slice;
        }

        current->size = size;
    } else {
        if (current->next != nullptr) {
            current->next->prev = current->prev;
        }

        if (current->prev != nullptr) {
            current->prev->next = current->next;
        } else {
            firstChunk = current->next;
        }
    }

    current->next = nullptr;
    current->prev = nullptr;

    lock.release();

    return (void*) (((uint32_t)current) + HEADER_SIZE);
}

FreeListMemoryManager::FLHeader *FreeListMemoryManager::merge(FLHeader *origin) {

    if(firstChunk == nullptr) {
        return nullptr;
    }

    FLHeader *tmp = origin;

    if (tmp->next != nullptr && ((uint32_t) tmp) + HEADER_SIZE + tmp->size == (uint32_t) tmp->next) {
        tmp->size += tmp->next->size + HEADER_SIZE;

        if (tmp->next->next != nullptr) {
            tmp->next->next->prev = tmp;
        }

        tmp->next = tmp->next->next;
    }

    tmp = tmp->prev;

    if (tmp != nullptr && ((uint32_t) tmp) + HEADER_SIZE + tmp->size == (uint32_t) origin) {
        tmp->size += tmp->next->size + HEADER_SIZE;

        if (tmp->next->next != nullptr) {
            tmp->next->next->prev = tmp;
        }

        tmp->next = tmp->next->next;

        origin = tmp;
    }

    return origin;

}
