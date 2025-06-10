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

#ifndef HHUOS_LIB_UTIL_FREELISTMEMORYMANAGER_H_
#define HHUOS_LIB_UTIL_FREELISTMEMORYMANAGER_H_

#include <stdint.h>
#include <stddef.h>

#include "async/Spinlock.h"
#include "base/HeapMemoryManager.h"
#include "base/String.h"
#include "reflection/Prototype.h"

namespace Util {
/// A memory manager that uses a doubly linked free list to manage a heap.
/// It allocates memory blocks using a first-fit algorithm.
/// Freed memory blocks are merged with adjacent free blocks to reduce fragmentation.
/// This class uses a spinlock to ensure thread safety.
class FreeListMemoryManager final : public HeapMemoryManager {
public:
    /// Create a new heap with a given range of memory.
    /// The start address is inclusive and the end address is exclusive.
    FreeListMemoryManager(void *startAddress, void *endAddress);

    /// A memory manager should not be copyable, since copies would operate on the same memory.
    FreeListMemoryManager(const FreeListMemoryManager &copy) = delete;

    /// A memory manager should not be copyable, since copies would operate on the same memory.
    FreeListMemoryManager& operator=(const FreeListMemoryManager &other) = delete;

    /// The free list memory manager class has no state that needs to be cleaned up,
    /// so the default destructor is sufficient.
    ~FreeListMemoryManager() override = default;

    /// Allocate a block of memory of a given size and alignment using the first-fit algorithm.
    /// If no sufficient block of memory is available, a panic is fired.
    [[nodiscard]] void* allocateMemory(size_t size, size_t alignment) override;

    /// Reallocate a previously allocated block of memory to a new size and alignment.
    /// If the current block can be extended, it is done in place.
    /// Otherwise, a new block is allocated and the content of the old block is copied into the new one.
    /// If no sufficient block of memory is available, a panic is fired.
    [[nodiscard]] void* reallocateMemory(void *ptr, size_t size, size_t alignment) override;

    /// Free a block of memory that was previously allocated by this memory manager.
    /// If possible, the freed block is merged with adjacent free blocks to reduce fragmentation.
    /// By default, the freed memory is unmapped to free up physical memory.
    /// This can be disabled by calling `disableAutomaticUnmapping()`.
    /// If the given pointer is outside the managed memory range, a panic is fired.
    /// However, if the pointer is nullptr, nothing happens.
    void freeMemory(void *ptr, size_t alignment) override;

    /// Get the total amount of memory managed by this memory manager.
    [[nodiscard]] size_t getTotalMemory() const override;

    /// Get the amount of free memory left in this memory manager.
    [[nodiscard]] size_t getFreeMemory() const override;

    /// Get the start address of the managed memory.
    [[nodiscard]] void* getStartAddress() const override;

    /// Get the end address of the managed memory.
    [[nodiscard]] void* getEndAddress() const override;

    /// Check if the spinlock is currently locked.
    [[nodiscard]] bool isLocked() const override;

    /// Stop unmapping freed memory.
    void disableAutomaticUnmapping();

    /// Check the integrity of the free list.
    [[nodiscard]] bool checkIntegrity() const;

private:
    /// Header of a free chunk of memory.
    /// It contains pointers to the previous and next chunk of memory, as well as the size of the chunk.
    struct FreeListHeader {
        FreeListHeader *prev;
        FreeListHeader *next;
        size_t size;
    };

    /// Implementation of the allocation algorithm, used in the `allocateMemory()` and `reallocateMemory()` functions.
    [[nodiscard]] void* allocAlgorithm(size_t size, size_t alignment, FreeListHeader *startChunk);

    /// Implementation of the free algorithm, that is used in the `freeMemory()` and `reallocateMemory()` functions.
    void freeAlgorithm(void *ptr);

    /// Find the next free chunk of memory that is large enough to satisfy the allocation request.
    /// The search starts at the given chunk and continues until the end of the free list.
    /// If no suitable chunk is found, nullptr is returned.
    [[nodiscard]] static FreeListHeader* findNext(FreeListHeader *start, size_t reqSize);

    /// Merge the given block with adjacent free blocks if possible.
    /// This method is called everytime a block is freed to reduce fragmentation.
    [[nodiscard]] FreeListHeader* merge(FreeListHeader *block) const;

    void *startAddress = nullptr;
    void *endAddress = nullptr;

    Async::Spinlock lock;
    FreeListHeader *firstChunk = nullptr;
    bool unmapFreedMemory = true;

    static constexpr size_t MIN_BLOCK_SIZE = 1;
    static constexpr size_t HEADER_SIZE = sizeof(FreeListHeader);
};

}

#endif
