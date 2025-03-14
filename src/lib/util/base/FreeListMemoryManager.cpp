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

#include "lib/util/base/Address.h"
#include "lib/interface.h"
#include "lib/util/base/Constants.h"
#include "FreeListMemoryManager.h"
#include "lib/util/base/Exception.h"

namespace Util {

void FreeListMemoryManager::initialize(uint8_t *startAddress, uint8_t *endAddress) {
    this->startAddress = startAddress;
    this->endAddress = endAddress;

    if (getTotalMemory() < sizeof(FreeListHeader)) {
        // Available memory is too small for a chunk
        Util::Exception::throwException(Util::Exception::ILLEGAL_STATE, "FreeListMemoryManager: Heap is too small!");
    } else {
        // set up first Chunk of memory
        firstChunk = reinterpret_cast<FreeListHeader*>(startAddress);
        firstChunk->size = getTotalMemory() - sizeof(FreeListHeader);
        firstChunk->next = nullptr;
        firstChunk->prev = nullptr;
    }
}

void* FreeListMemoryManager::allocateMemory(uint32_t size, uint32_t alignment) {
    lock.acquire();
    void *ret = allocAlgorithm(size, alignment, firstChunk);
    lock.release();

    if (size > 0 && ret == nullptr) {
        Util::Exception::throwException(Exception::OUT_OF_MEMORY, "FreeListMemoryManager: Allocation failed!");
    } else if (size > 0 && (ret < getStartAddress() || ret > getEndAddress())) {
        Util::Exception::throwException(Exception::ILLEGAL_STATE, "FreeListMemoryManager: Allocated memory outside of heap boundaries!");
    }

    return ret;
}

void FreeListMemoryManager::freeMemory(void *ptr, [[maybe_unused]] uint32_t alignment) {
    lock.acquire();
    freeAlgorithm(ptr);
    lock.release();
}

FreeListMemoryManager::FreeListHeader* FreeListMemoryManager::findNext(FreeListHeader *start, uint32_t reqSize) {
    // set start point
    FreeListHeader *current = start;

    // run through free list and look for free block with correct size
    while (current != nullptr) {
        if (current->size >= reqSize) {
            return current;
        }

        current = current->next;
    }

    return current;
}

void* FreeListMemoryManager::allocAlgorithm(uint32_t size, uint32_t alignment, FreeListHeader *startChunk) {
    // check for invalid requests
    if (size == 0) {
        return nullptr;
    }

    if (startChunk == nullptr) {
        startChunk = firstChunk;
    }

    // align requested size to 4 byte
    size = Util::Address(size).alignUp(sizeof(uint32_t)).get();

    FreeListHeader *current = startChunk;
    FreeListHeader *aligned;

    // run through list and look for memory block
    while (current != nullptr) {
        if (current->size >= size) {
            auto data = (reinterpret_cast<uint8_t*>(current)) + HEADER_SIZE;
            auto alignedData = reinterpret_cast<uint8_t*>(Util::Address(data).alignUp(alignment).get());

            // Found free Memory Block with required alignment
            if (data == alignedData) {
                break;
            }

            // We want to place the header in front of alignedData, so we need to check, if there is enough space
            // If the space is not sufficient, we align the address up until it is
            while (alignedData - HEADER_SIZE < data + MIN_BLOCK_SIZE) {
                alignedData += alignment;
            }

            aligned = reinterpret_cast<FreeListHeader*>(alignedData - HEADER_SIZE);

            // Check if current block has enough free data space to fit in the aligned block
            if (reinterpret_cast<uint8_t*>(aligned) + HEADER_SIZE + size <= reinterpret_cast<uint8_t*>(current) + HEADER_SIZE + current->size) {
                aligned->size = reinterpret_cast<uint8_t*>(current) + current->size - reinterpret_cast<uint8_t*>(aligned);
                current->size = reinterpret_cast<uint8_t*>(aligned) - (reinterpret_cast<uint8_t*>(current) + HEADER_SIZE);

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
        auto slice = reinterpret_cast<FreeListHeader*>(reinterpret_cast<uint8_t*>(current) + HEADER_SIZE + size);

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

    return reinterpret_cast<void*>(reinterpret_cast<uint8_t*>(current) + HEADER_SIZE);
}

void FreeListMemoryManager::freeAlgorithm(void *ptr) {
    // check for nullpointer
    if (ptr == nullptr) {
        return;
    }
    // check if address points to valid memory for this manager
    if (ptr < startAddress || ptr > endAddress) {
        Util::Exception::throwException(Exception::OUT_OF_BOUNDS, "free: Trying to free memory outside of heap boundaries");
    }

    // get pointer to header of allocated block
    auto header = reinterpret_cast<FreeListHeader*>(reinterpret_cast<uint8_t*>(ptr) - HEADER_SIZE);

    // Place free block at the right position in free list
    // if there is no free list -> initializeScene one
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
        FreeListHeader *tmp = firstChunk;
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
    auto *mergedHeader = merge(header);

    // if the free chunk has more than 4KB of memory, a page can possibly be unmapped
    if (unmapFreedMemory && mergedHeader->size >= Util::PAGESIZE && isMemoryManagementInitialized()) {
        auto mergedAddress = reinterpret_cast<uint8_t*>(mergedHeader);
        auto size = HEADER_SIZE + mergedHeader->size;

        // try to unmap the free memory, not the list header!
        unmap(mergedAddress + HEADER_SIZE, size / Util::PAGESIZE, 8);
    }
}

/**
 * Merge all free blocks of free memory if possible
 */
FreeListMemoryManager::FreeListHeader* FreeListMemoryManager::merge(FreeListHeader *origin) {
    if (firstChunk == nullptr) {
        return nullptr;
    }

    auto *tmp = origin;

    // merge with next block if possible
    if (tmp->next != nullptr && (reinterpret_cast<uint8_t*>(tmp) + HEADER_SIZE + tmp->size == reinterpret_cast<uint8_t*>(tmp->next))) {
        tmp->size += tmp->next->size + HEADER_SIZE;

        if (tmp->next->next != nullptr) {
            tmp->next->next->prev = tmp;
        }

        tmp->next = tmp->next->next;
    }

    tmp = tmp->prev;

    // merge with previous block if possible
    if (tmp != nullptr && reinterpret_cast<uint8_t*>(tmp) + HEADER_SIZE + tmp->size == reinterpret_cast<uint8_t*>(origin)) {
        tmp->size += tmp->next->size + HEADER_SIZE;

        if (tmp->next->next != nullptr) {
            tmp->next->next->prev = tmp;
        }

        tmp->next = tmp->next->next;
        origin = tmp;
    }

    return origin;
}

void* FreeListMemoryManager::reallocateMemory(void *ptr, uint32_t size, uint32_t alignment) {
    void *ret = nullptr;

    if (size == 0) {
        freeMemory(ptr, 0);
        return ret;
    }

    auto oldHeader = reinterpret_cast<FreeListHeader*>(reinterpret_cast<uint8_t*>(ptr) - HEADER_SIZE);

    if (oldHeader->size == size) {
        if (alignment == 0 || (uint32_t) ptr % alignment == 0) {
            return ptr;
        } else {
            ret = allocateMemory(size, alignment);
        }
    } else if (size < oldHeader->size) {
        if (alignment == 0 || (uint32_t) ptr % alignment == 0) {
            if (oldHeader->size - size > MIN_BLOCK_SIZE + HEADER_SIZE) {
                auto newHeader = reinterpret_cast<FreeListHeader*>(reinterpret_cast<uint8_t*>(ptr) + size);
                newHeader->size = oldHeader->size - size - HEADER_SIZE;

                freeMemory(reinterpret_cast<uint8_t*>(newHeader) + HEADER_SIZE, 0);
                oldHeader->size = size;

                return ptr;
            } else {
                return ptr;
            }
        } else {
            ret = allocateMemory(size, alignment);
        }
    } else {
        if (alignment == 0 || (uint32_t) ptr % alignment == 0) {
            lock.acquire();
            FreeListHeader *currentChunk = firstChunk;
            FreeListHeader *returnChunk = nullptr;

            do {
                if (currentChunk->size >= size) {
                    if (returnChunk == nullptr || currentChunk->size < returnChunk->size) {
                        returnChunk = currentChunk;
                    }
                }

                currentChunk = currentChunk->next;
            } while (currentChunk != nullptr && currentChunk < ptr);

            if (currentChunk != nullptr) {
                if (((uint32_t) ptr + oldHeader->size == (uint32_t) currentChunk) && (oldHeader->size + currentChunk->size + HEADER_SIZE >= size)) {

                    currentChunk->prev->next = currentChunk->next;

                    if (currentChunk->next != nullptr) {
                        currentChunk->next->prev = currentChunk->prev;
                    }

                    oldHeader->size += currentChunk->size + HEADER_SIZE;

                    if (oldHeader->size - size > HEADER_SIZE + MIN_BLOCK_SIZE) {
                        auto newHeader = reinterpret_cast<FreeListHeader*>(reinterpret_cast<uint8_t*>(ptr) + size);
                        newHeader->size = oldHeader->size - size - HEADER_SIZE;

                        freeAlgorithm(reinterpret_cast<uint8_t*>(newHeader) + HEADER_SIZE);

                        oldHeader->size = size;
                    }

                    lock.release();
                    return ptr;
                } else {
                    ret = allocAlgorithm(size, alignment, returnChunk);
                }
            } else {
                ret = allocAlgorithm(size, alignment, returnChunk);
            }

            lock.release();
        } else {
            ret = allocateMemory(size, alignment);
        }
    }

    if (ret != nullptr) {
        if (ret < getStartAddress() || ret > getEndAddress()) {
            Util::Exception::throwException(Exception::OUT_OF_BOUNDS, "realloc: Allocated memory outside of heap boundaries");
        }

        Util::Address(ret).copyRange(Util::Address(ptr), (size < oldHeader->size) ? size : oldHeader->size);
        freeMemory(ptr, 0);
    }

    return ret;
}

uint8_t* FreeListMemoryManager::getStartAddress() const {
    return startAddress;
}

uint32_t FreeListMemoryManager::getTotalMemory() const {
    return endAddress - startAddress + 1;
}

uint32_t FreeListMemoryManager::getFreeMemory() const {
     uint32_t freeMemory = 0;

    FreeListHeader *current = firstChunk;
    while (current != nullptr) {
        freeMemory  += current->size;
        current = current->next;
    }

    return freeMemory;
}

uint8_t* FreeListMemoryManager::getEndAddress() const {
    return endAddress;
}

void FreeListMemoryManager::disableAutomaticUnmapping() {
    unmapFreedMemory = false;
}

bool FreeListMemoryManager::isLocked() const {
    return lock.isLocked();
}

}
