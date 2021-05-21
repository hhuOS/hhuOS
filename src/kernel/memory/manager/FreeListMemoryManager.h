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

#ifndef __FREELISTMEMORYMANAGER_H__
#define __FREELISTMEMORYMANAGER_H__

#include <cstdint>
#include "lib/util/async/Spinlock.h"
#include "HeapMemoryManager.h"

namespace Kernel {

/**
 * Memory manager, that uses a doubly linked list of free chunks of memory.
 *
 * This memory manager allows allocation and reallocation of memory with or without an alignment.
 *
 * @author Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schoettner
 * @date 2018
 */
class FreeListMemoryManager : public HeapMemoryManager {

public:

    PROTOTYPE_IMPLEMENT_CLONE(FreeListMemoryManager);

    /**
     * Constructor.
     */
    FreeListMemoryManager() = default;

    /**
     * Copy constructor.
     */
    FreeListMemoryManager(const FreeListMemoryManager &copy) = delete;

    /**
     * Assignment operator.
     */
    FreeListMemoryManager& operator=(const FreeListMemoryManager &other) = delete;

    /**
     * Destructor.
     */
    ~FreeListMemoryManager() override = default;

    /**
     * Overriding function from MemoryManager.
     */
    void initialize(uint32_t startAddress, uint32_t endAddress) override;

    /**
     * Overriding function from MemoryManager.
     */
    Util::Memory::String getClassName() override;

    /**
     * Overriding function from MemoryManager.
     */
    void *alloc(uint32_t size) override;

    /**
     * Overriding function from MemoryManager.
     */
    void *allignedAlloc(uint32_t size, uint32_t alignment) override;

    /**
     * Overriding function from MemoryManager.
     */
    void *realloc(void *ptr, uint32_t size) override;

    /**
     * Overriding function from MemoryManager.
     */
    void *realloc(void *ptr, uint32_t size, uint32_t alignment) override;

    /**
     * Overriding function from MemoryManager.
     */
    void free(void *ptr) override;

    /**
     * Overriding function from MemoryManager.
     */
    void alignedFree(void *ptr, uint32_t alignment) override;

private:
    /**
     * Header of an element in the doubly linked list, which is used to manage the free chunks of memory.
     */
    struct FreeListHeader {
        FreeListHeader *prev;
        FreeListHeader *next;
        uint32_t size;
    };

    /**
     * Implementation of the allocation algorithm, that is used in the allignedAlloc-functions.
     *
     * The first-fit algorithm is used to search for a fitting chunk of free memory.
     *
     * @param size Size of the chunk of memory to be allocated
     * @param alignment Alignment, that chunk of memory should have
     * @param startChunk The chunk of free memory from which to start searching for a fitting chunk
     *
     * @return Pointer to the allocated chunk of memory or nullptr if no chunk with the required size is available
     */
    void *allocAlgorithm(uint32_t size, uint32_t alignment, FreeListHeader *startChunk);

    /**
     * Implementation of the free algorithm, that is used in the free-functions.
     *
     * @param ptr Pointer to the chunk of memory to be freed
     */
    void freeAlgorithm(void *ptr);

private:

    Util::Async::Spinlock lock;

    FreeListHeader *firstChunk = nullptr;

    /**
     * Find the next chunk of memory with a required size.
     *
     * @param start Pointer to the chunk of memory from where to start the search
     * @param reqSize Minimal size that is required for the chunk
     *
     * @return Header of the free chunk with the required size or nullptr, if none is found
     */
    static FreeListHeader *findNext(FreeListHeader *start, uint32_t reqSize);

    /**
     * Merge a chunk of free memory with it's neighbours, if possible.
     *
     * @param origin Chunk of free memory to be merged
     */
    FreeListHeader *merge(FreeListHeader *origin);

    static const constexpr char *CLASS_NAME = "Kernel::FreeListMemoryManager";

    static const constexpr uint32_t MIN_BLOCK_SIZE = 4;

    static const constexpr uint32_t HEADER_SIZE = sizeof(FreeListHeader);
};

}

#endif
