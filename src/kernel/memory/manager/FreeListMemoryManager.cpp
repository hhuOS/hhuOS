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

#include <util/memory/Address.h>
#include "kernel/memory/Paging.h"
#include "FreeListMemoryManager.h"

#include "kernel/core/Management.h"

namespace Kernel {

FreeListMemoryManager::FreeListMemoryManager() : MemoryManager() {

}

FreeListMemoryManager::FreeListMemoryManager(const FreeListMemoryManager &copy) : FreeListMemoryManager() {

}

void FreeListMemoryManager::init(uint32_t memoryStartAddress, uint32_t memoryEndAddress, bool doUnmap) {
    MemoryManager::init(memoryStartAddress, memoryEndAddress, doUnmap);

    if (freeMemory < sizeof(FLHeader)) {
        // Available Kernel-Memory is too small for a Chunk
        firstChunk = nullptr;
    } else {
        // set up first Chunk of memory
        firstChunk = (FLHeader *) memoryStartAddress;
        freeMemory -= sizeof(FLHeader);
        firstChunk->size = freeMemory;
        firstChunk->next = nullptr;
        firstChunk->prev = nullptr;
    }
}

Util::Memory::String FreeListMemoryManager::getTypeName() {
    return TYPE_NAME;
}

void *FreeListMemoryManager::alloc(uint32_t size) {
    // Allocate memory without alignment
    return alloc(size, 0);
}

void FreeListMemoryManager::free(void *ptr) {
    free(ptr, 0);
}

void FreeListMemoryManager::free(void *ptr, uint32_t alignment) {
    lock.acquire();

    freeAlgorithm(ptr);

    lock.release();
}

FreeListMemoryManager::FLHeader *FreeListMemoryManager::findNext(FLHeader *start, uint32_t reqSize) {
    // set start point
    FLHeader *current = start;

    // run through free list and look for free block with correct size
    while (current != nullptr) {
        if (current->size >= reqSize) {
            return current;
        }

        current = current->next;
    }

    return current;
}

void *FreeListMemoryManager::allocAlgorithm(uint32_t size, uint32_t alignment, FLHeader *startChunk) {
    // check for invalid requests
    if (size == 0) {
        return nullptr;
    }

    if (startChunk == nullptr) {
        startChunk = firstChunk;
    }

    // align requested size to 4 byte
    size = Util::Memory::Address<uint32_t>(size).alignUp(sizeof(uint32_t)).get();

    FLHeader *current = startChunk;
    FLHeader *aligned = nullptr;

    // run through list and look for memory block
    while (current != nullptr) {

        if (current->size >= size) {

            uint32_t data = ((uint32_t) current) + HEADER_SIZE;
            uint32_t alignedData = Util::Memory::Address(data).alignUp(alignment).get();

            // Found free Memory Block with required alignment
            if (data == alignedData) {
                break;
            }

            // Make sure the aligned Memory Block's header
            // does not overlap with the current one
            aligned = (FLHeader * )(Util::Memory::Address(data + HEADER_SIZE + MIN_BLOCK_SIZE).alignUp(alignment).get() - HEADER_SIZE);

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

    // Check if the chosen chunk can be sliced in two parts
    if (current->size - size >= MIN_BLOCK_SIZE + HEADER_SIZE) {
        auto slice = (FLHeader * )(((uint32_t) current) + HEADER_SIZE + size);

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

        freeMemory -= HEADER_SIZE;
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

    freeMemory -= current->size;

    return (void *) (((uint32_t) current) + HEADER_SIZE);
}

void FreeListMemoryManager::freeAlgorithm(void *ptr) {
    // check for nullpointer
    if (ptr == nullptr) {
        return;
    }
    // check if address points to valid memory for this manager
    if ((uint32_t) ptr < memoryStartAddress || (uint32_t) ptr > memoryEndAddress) {
        return;
    }

    // get pointer to header of allocated block
    auto header = (FLHeader * )((uint8_t *) ptr - HEADER_SIZE);

    freeMemory += header->size;

    // Place free block at the right position in free list

    // if there is no free list -> create one
    if (firstChunk == nullptr) {
        firstChunk = header;
        // if freed block is before first entry of free list -> freed block is new anchor
    } else if (header < firstChunk) {
        header->next = firstChunk;
        header->prev = nullptr;
        firstChunk->prev = header;
        firstChunk = header;
        // else: search for correct position of freed block in free list and place it there
    } else {
        FLHeader *tmp = firstChunk;
        while (tmp != nullptr) {

            if (header > tmp && (header < tmp->next || tmp->next == nullptr)) {
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

    // merge freed block of memory with neighbours if possible
    FLHeader *mergedHeader = merge(header);

    // if the free chunk has more than 4kb of memory, a page can possibly be unmapped
    if (doUnmap && mergedHeader->size >= PAGESIZE && Management::isInitialized()) {
        auto addr = (uint32_t) mergedHeader;
        auto chunkEndAddr = addr + (HEADER_SIZE + mergedHeader->size);

        // try to unmap the free memory, not the list header!
        Management::getInstance().unmap(addr + HEADER_SIZE, chunkEndAddr - 1);
    }
}

/**
 * Allocate aligned memory block with given size.
 */
void *FreeListMemoryManager::alloc(uint32_t size, uint32_t alignment) {
    lock.acquire();

    void *ret = allocAlgorithm(size, alignment, firstChunk);

    lock.release();

    return ret;
}

/**
 * Merge all free blocks of free memory if possible
 */
FreeListMemoryManager::FLHeader *FreeListMemoryManager::merge(FLHeader *origin) {

    if (firstChunk == nullptr) {
        return nullptr;
    }

    FLHeader *tmp = origin;

    // merge with next block if possible
    if (tmp->next != nullptr && ((uint32_t) tmp) + HEADER_SIZE + tmp->size == (uint32_t) tmp->next) {
        tmp->size += tmp->next->size + HEADER_SIZE;

        if (tmp->next->next != nullptr) {
            tmp->next->next->prev = tmp;
        }

        tmp->next = tmp->next->next;

        freeMemory += HEADER_SIZE;
    }

    tmp = tmp->prev;

    // merge with previous block if possible
    if (tmp != nullptr && ((uint32_t) tmp) + HEADER_SIZE + tmp->size == (uint32_t) origin) {
        tmp->size += tmp->next->size + HEADER_SIZE;

        if (tmp->next->next != nullptr) {
            tmp->next->next->prev = tmp;
        }

        tmp->next = tmp->next->next;

        origin = tmp;

        freeMemory += HEADER_SIZE;
    }

    return origin;

}

void *FreeListMemoryManager::realloc(void *ptr, uint32_t size) {
    return realloc(ptr, size, 0);
}

void *FreeListMemoryManager::realloc(void *ptr, uint32_t size, uint32_t alignment) {
    void *ret = nullptr;

    if (size == 0) {
        free(ptr);

        return ret;
    }

    auto oldHeader = (FLHeader * )((uint32_t) ptr - HEADER_SIZE);

    if (oldHeader->size == size) {
        if (alignment == 0 || (uint32_t) ptr % alignment == 0) {
            return ptr;
        } else {
            ret = alloc(size, alignment);
        }
    } else if (size < oldHeader->size) {
        if (alignment == 0 || (uint32_t) ptr % alignment == 0) {
            if (oldHeader->size - size > MIN_BLOCK_SIZE + HEADER_SIZE) {
                auto newHeader = (FLHeader * )((uint32_t) ptr + size);
                newHeader->size = oldHeader->size - size - HEADER_SIZE;

                free(newHeader + HEADER_SIZE);

                oldHeader->size = size;

                return ptr;
            } else {
                return ptr;
            }
        } else {
            ret = alloc(size, alignment);
        }
    } else {
        if (alignment == 0 || (uint32_t) ptr % alignment == 0) {
            FLHeader *currentChunk = firstChunk;
            FLHeader *returnChunk = nullptr;

            lock.acquire();

            do {
                if (currentChunk->size >= size) {
                    if (returnChunk == nullptr || currentChunk->size < returnChunk->size) {
                        returnChunk = currentChunk;
                    }
                }

                currentChunk = currentChunk->next;
            } while (currentChunk != nullptr && currentChunk < ptr);

            if (currentChunk != nullptr) {
                if (((uint32_t) ptr + oldHeader->size == (uint32_t) currentChunk) &&
                    (oldHeader->size + currentChunk->size + HEADER_SIZE >= size)) {
                    freeMemory -= currentChunk->size;

                    currentChunk->prev->next = currentChunk->next;

                    if (currentChunk->next != nullptr) {
                        currentChunk->next->prev = currentChunk->prev;
                    }

                    oldHeader->size += currentChunk->size + HEADER_SIZE;

                    if (oldHeader->size - size > HEADER_SIZE + MIN_BLOCK_SIZE) {
                        auto newHeader = (FLHeader * )((uint32_t) ptr + size);
                        newHeader->size = oldHeader->size - size - HEADER_SIZE;

                        freeAlgorithm((uint8_t *) newHeader + HEADER_SIZE);

                        oldHeader->size = size;
                    }

                    lock.release();

                    return ptr;
                } else {
                    ret = allocAlgorithm(size, alignment, returnChunk);
                }
            } else {
                ret = allocAlgorithm(size, alignment, returnChunk);

                lock.release();
            }

            lock.release();
        } else {
            ret = alloc(size, alignment);
        }
    }

    if (ret != nullptr) {
        Util::Memory::Address<uint32_t>(ret).copyRange(Util::Memory::Address<uint32_t>(ptr), (size < oldHeader->size) ? size : oldHeader->size);

        free(ptr);
    }

    return ret;
}

}
