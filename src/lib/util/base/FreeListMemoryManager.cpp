/*
 * Copyright (C) 2017-2025 Heinrich Heine University Düsseldorf,
 * Institute of Computer Science, Department Operating Systems
 * Main developers: Christian Gesse <christian.gesse@hhu.de>, Fabian Ruhland <ruhland@hhu.de>
 * Original development team: Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schöttner
 * This project has been supported by several students.
 * A full list of integrated student theses can be found here: https://github.com/hhuOS/hhuOS/wiki/Student-theses
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

#include "FreeListMemoryManager.h"

#include "lib/interface.h"
#include "base/Address.h"
#include "base/Constants.h"
#include "base/Panic.h"

namespace Util {

FreeListMemoryManager::FreeListMemoryManager(void *startAddress, void *endAddress) :
    startAddress(startAddress), endAddress(endAddress)
{
    if (getTotalMemory() < sizeof(FreeListHeader)) {
        // Available memory is too small to fit even a single header
        Panic::fire(Panic::ILLEGAL_STATE, "FreeListMemoryManager: Heap is too small!");
    }

    // Set up the first chunk of memory containing the whole heap
    firstChunk = static_cast<FreeListHeader*>(startAddress);
    firstChunk->size = getTotalMemory() - sizeof(FreeListHeader);
    firstChunk->next = nullptr;
    firstChunk->prev = nullptr;
}

void* FreeListMemoryManager::allocateMemory(const size_t size, const size_t alignment) {
    lock.acquire();
    void *ret = allocAlgorithm(size, alignment, firstChunk);
    lock.release();

    if (size > 0 && ret == nullptr) {
        // No suitable memory block was found
        Panic::fire(Panic::OUT_OF_MEMORY, "FreeListMemoryManager: Allocation failed!");
    }

    if (size > 0 && (ret < getStartAddress() || ret > getEndAddress())) {
        // Sanity check, should never happen if the memory manager is used correctly
        Panic::fire(Panic::ILLEGAL_STATE,
            "FreeListMemoryManager: Allocated memory outside of heap boundaries!");
    }

    return ret;
}

void FreeListMemoryManager::freeMemory(void *ptr, [[maybe_unused]] const size_t alignment) {
    lock.acquire();
    freeAlgorithm(ptr);
    lock.release();
}

FreeListMemoryManager::FreeListHeader* FreeListMemoryManager::findNext(FreeListHeader *start, const size_t reqSize) {
    auto *current = start;

    // Go through free list and look for a block of sufficient size
    while (current != nullptr) {
        if (current->size >= reqSize) {
            return current;
        }

        current = current->next;
    }

    return current;
}

void* FreeListMemoryManager::allocAlgorithm(const size_t size, const size_t alignment, FreeListHeader *startChunk) {
    if (size == 0) {
        return nullptr;
    }

    if (startChunk == nullptr) {
        startChunk = firstChunk;
    }

    auto *current = startChunk;

    // Go through list and look for a suitable memory block
    while (current != nullptr) {
        if (current->size >= size) {
            const auto data = reinterpret_cast<uint8_t*>(current) + HEADER_SIZE;
            auto alignedData = reinterpret_cast<uint8_t*>(Address(data).alignUp(alignment).get());

            // Found free memory block with required alignment and sufficient space
            if (data == alignedData) {
                break;
            }

            // Check if there is enough free space before alignedData to fit in a header.
            // If not, align it to the next possible address until a valid header can be created.
            while (alignedData - HEADER_SIZE < data + MIN_BLOCK_SIZE) {
                alignedData += alignment;
            }

            auto *aligned = reinterpret_cast<FreeListHeader*>(alignedData - HEADER_SIZE);

            // Check if the aligned block still has sufficient space
            if (reinterpret_cast<uint8_t*>(aligned) + HEADER_SIZE + size <=
                reinterpret_cast<uint8_t*>(current) + HEADER_SIZE + current->size)
            {
                // Create a new header for the aligned block and update the current block to hold the remaining space
                aligned->size = reinterpret_cast<uint8_t*>(current)
                                + current->size
                                - reinterpret_cast<uint8_t*>(aligned);
                current->size = reinterpret_cast<uint8_t*>(aligned)
                                - (reinterpret_cast<uint8_t*>(current) + HEADER_SIZE);

                // Chain the new header into the free list with the current block as its predecessor
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

        // The current block was not suitable. Continue with the next one.
        current = findNext(current->next, size);
    }

    // No suitable block found
    if (current == nullptr) {
        return nullptr;
    }

    // Check if the chosen chunk can be split in two parts
    if (current->size - size >= MIN_BLOCK_SIZE + HEADER_SIZE) {
        // Create a new header for the remaining free memory
        const auto slice = reinterpret_cast<FreeListHeader*>(reinterpret_cast<uint8_t*>(current) + HEADER_SIZE + size);

        // Chain the new header into the free list
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
        // The current block is too small to be split, we just remove it from the free list
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

    return reinterpret_cast<uint8_t*>(current) + HEADER_SIZE;
}

void FreeListMemoryManager::freeAlgorithm(void *ptr) {
    if (ptr == nullptr) {
        return;
    }

    // Check if address points to valid memory for this manager
    if (ptr < startAddress || ptr > endAddress) {
        Panic::fire(Panic::OUT_OF_BOUNDS, "free: Trying to free memory outside of heap boundaries");
    }

    // Get pointer to header of allocated block (always in front of the data)
    const auto header = reinterpret_cast<FreeListHeader*>(static_cast<uint8_t*>(ptr) - HEADER_SIZE);

    // Place free block at the right position in free list.
    if (firstChunk == nullptr) { // Empty list -> Freed block is the new anchor
        firstChunk = header;
    } else if (header < firstChunk) { // Freed block lies before the current anchor
        header->next = firstChunk;
        header->prev = nullptr;
        firstChunk->prev = header;
        firstChunk = header;
    } else { // Freed block lies somewhere in the list -> We need to find the right position
        FreeListHeader *current = firstChunk;
        while (current != nullptr) {
            if (header > current && (header < current->next || current->next == nullptr)) {
                header->next = current->next;
                header->prev = current;

                if (header->next != nullptr) {
                    header->next->prev = header;
                }

                header->prev->next = header;
                break;
            }

            current = current->next;
        }
    }

    // Merge the freed block with its neighbours if possible
    auto *mergedHeader = merge(header);

    // If the free memory block is larger than a page try to unmap it (freeing up physical memory)
    if (unmapFreedMemory && mergedHeader->size >= PAGESIZE && isMemoryManagementInitialized()) {
        const auto mergedAddress = reinterpret_cast<uint8_t*>(mergedHeader);
        const auto size = HEADER_SIZE + mergedHeader->size;

        unmap(mergedAddress + HEADER_SIZE, size / PAGESIZE, 8);
    }
}

FreeListMemoryManager::FreeListHeader* FreeListMemoryManager::merge(FreeListHeader *block) const {
    if (firstChunk == nullptr) {
        return nullptr;
    }

    auto *current = block;

    // Merge with next block if possible
    if (current->next != nullptr &&
        reinterpret_cast<uint8_t*>(current) + HEADER_SIZE + current->size == reinterpret_cast<uint8_t*>(current->next))
    {
        current->size += current->next->size + HEADER_SIZE;

        if (current->next->next != nullptr) {
            current->next->next->prev = current;
        }

        current->next = current->next->next;
    }

    current = current->prev;

    // Merge with previous block if possible
    if (current != nullptr &&
        reinterpret_cast<uint8_t*>(current) + HEADER_SIZE + current->size == reinterpret_cast<uint8_t*>(block))
    {
        current->size += current->next->size + HEADER_SIZE;

        if (current->next->next != nullptr) {
            current->next->next->prev = current;
        }

        current->next = current->next->next;
        block = current;
    }

    return block;
}

void* FreeListMemoryManager::reallocateMemory(void *ptr, const size_t size, const size_t alignment) {
    void *ret = nullptr;

    // realloc(ptr, 0) is equivalent to free(ptr)
    if (size == 0) {
        freeMemory(ptr, 0);
        return ret;
    }

    const auto oldHeader = reinterpret_cast<FreeListHeader*>(static_cast<uint8_t*>(ptr) - HEADER_SIZE);

    if (oldHeader->size == size) {
        if (alignment == 0 || reinterpret_cast<uint32_t>(ptr) % alignment == 0) {
            // Size and alignment are already correct
            return ptr;
        }

        ret = allocateMemory(size, alignment);
    } else if (size < oldHeader->size) {
        if (alignment == 0 || reinterpret_cast<uint32_t>(ptr) % alignment == 0) {
            // The new size is smaller than the old size, so we can shrink the block
            if (oldHeader->size - size > MIN_BLOCK_SIZE + HEADER_SIZE) {
                // The remaining block is large enough to be split
                const auto newHeader = reinterpret_cast<FreeListHeader*>(static_cast<uint8_t*>(ptr) + size);
                newHeader->size = oldHeader->size - size - HEADER_SIZE;

                freeMemory(reinterpret_cast<uint8_t*>(newHeader) + HEADER_SIZE, 0);
                oldHeader->size = size;

                return ptr;
            }

            // The remaining block is too small to be split, so we just return the original pointer
            return ptr;
        }

        ret = allocateMemory(size, alignment);
    } else {
        // The new size is larger than the old size.
        // We check if the next block is free and can be merged with the current block.
        // This way we can avoid allocating a new block and copying the data.
        // This only works if the current block is aligned correctly.
        if (alignment == 0 || reinterpret_cast<uint32_t>(ptr) % alignment == 0) {
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
                if (reinterpret_cast<uint32_t>(ptr) + oldHeader->size == reinterpret_cast<uint32_t>(currentChunk) &&
                    oldHeader->size + currentChunk->size + HEADER_SIZE >= size)
                {
                    currentChunk->prev->next = currentChunk->next;

                    if (currentChunk->next != nullptr) {
                        currentChunk->next->prev = currentChunk->prev;
                    }

                    oldHeader->size += currentChunk->size + HEADER_SIZE;

                    if (oldHeader->size - size > HEADER_SIZE + MIN_BLOCK_SIZE) {
                        const auto newHeader = reinterpret_cast<FreeListHeader*>(static_cast<uint8_t*>(ptr) + size);
                        newHeader->size = oldHeader->size - size - HEADER_SIZE;
                        freeAlgorithm(reinterpret_cast<uint8_t*>(newHeader) + HEADER_SIZE);
                        oldHeader->size = size;
                    }

                    lock.release();
                    return ptr;
                }

                ret = allocAlgorithm(size, alignment, returnChunk);
            } else {
                ret = allocAlgorithm(size, alignment, returnChunk);
            }

            lock.release();
        } else {
            ret = allocateMemory(size, alignment);
        }
    }

    if (ret != nullptr) { // Copy the data from the old block to the new block
        if (ret < getStartAddress() || ret > getEndAddress()) {
            Panic::fire(Panic::OUT_OF_BOUNDS, "realloc: Allocated memory outside of heap boundaries");
        }

        Address(ret).copyRange(Address(ptr), size < oldHeader->size ? size : oldHeader->size);
        freeMemory(ptr, 0);
    }

    return ret;
}

void* FreeListMemoryManager::getStartAddress() const {
    return startAddress;
}

size_t FreeListMemoryManager::getTotalMemory() const {
    return reinterpret_cast<uint32_t>(endAddress) - reinterpret_cast<uint32_t>(startAddress);
}

size_t FreeListMemoryManager::getFreeMemory() const {
     size_t freeMemory = 0;

    // Go through the list and sum up the sizes of all free blocks
    const auto *current = firstChunk;
    while (current != nullptr) {
        freeMemory  += current->size;
        current = current->next;
    }

    return freeMemory;
}

void* FreeListMemoryManager::getEndAddress() const {
    return endAddress;
}

void FreeListMemoryManager::disableAutomaticUnmapping() {
    unmapFreedMemory = false;
}

bool FreeListMemoryManager::checkIntegrity() const {
    if (firstChunk->prev != nullptr) {
        return false;
    }

    const auto *current = firstChunk;
    while (current != nullptr) {
        if (current < startAddress || current >= endAddress) {
            return false;
        }

        if (current->next != nullptr && current->next < current->prev) {
            return false;
        }

        if (current->next != nullptr && current->next < current) {
            return false;
        }

        current = current->next;
    }

    return true;
}

bool FreeListMemoryManager::isLocked() const {
    return lock.isLocked();
}

}
