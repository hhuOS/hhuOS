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

#ifndef __FREELISTMEMORYMANAGER_H__
#define __FREELISTMEMORYMANAGER_H__

#include <cstdint>
#include "lib/lock/Spinlock.h"
#include "MemoryManager.h"

/**
 * Memory manager, that uses a doubly linked list of free chunks of memory.
 *
 * This memory manager allows allocation and reallocation of memory with or without an alignment.
 *
 * @author Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schoettner
 * @date 2018
 */
class FreeListMemoryManager : public MemoryManager {

private:

    /**
     * Header of an element in the doubly linked list, which is used to manage the free chunks of memory.
     */
    struct FLHeader {
        FLHeader* prev;
        FLHeader* next;
        uint32_t size;
    };

    Spinlock lock;

    FLHeader* firstChunk = nullptr;

    /**
     * Find the next chunk of memory with a required size.
     *
     * @param start Pointer to the chunk of memory from where to start the search
     * @param reqSize Minimal size that is required for the chunk
     *
     * @return Header of the free chunk with the required size or nullptr, if none is found
     */
    FLHeader *findNext(FLHeader *start, uint32_t reqSize);

    /**
     * Merge a chunk of free memory with it's neighbours, if possible.
     *
     * @param origin Chunk of free memory to be merged
     */
    FLHeader *merge(FLHeader *origin);

    static const constexpr char *NAME = "FreeListMemoryManager";

    static const constexpr uint32_t MIN_BLOCK_SIZE = 4;

    static const constexpr uint32_t HEADER_SIZE = sizeof(FLHeader);

private:

    /**
     * Implementation of the allocation algorithm, that is used in the alloc-functions.
     *
     * The first-fit algorithm is used to search for a fitting chunk of free memory.
     *
     * @param size Size of the chunk of memory to be allocated
     * @param alignment Alignment, that chunk of memory should have
     * @param startChunk The chunk of free memory from which to start searching for a fitting chunk
     *
     * @return Pointer to the allocated chunk of memory or nullptr if no chunk with the required size is available
     */
    void* allocAlgorithm(uint32_t size, uint32_t alignment, FLHeader *startChunk);

    /**
     * Implementation of the free algorithm, that is used in the free-functions.
     *
     * @param ptr Pointer to the chunk of memory to be freed
     */
    void freeAlgorithm(void *ptr);

public:

    PROTOTYPE_IMPLEMENT_CLONE(FreeListMemoryManager);

    /**
     * Constructor.
     */
    FreeListMemoryManager();

    /**
     * Copy-constructor.
     */
    FreeListMemoryManager(const FreeListMemoryManager &copy);

    /**
     * Destructor.
     */
    ~FreeListMemoryManager() override = default;

    /**
     * Overriding function from MemoryManager.
     */
    void init(uint32_t memoryStartAddress, uint32_t memoryEndAddress, bool doUnmap) override;

    /**
     * Overriding function from MemoryManager.
     */
    String getName() override;

    /**
     * Overriding function from MemoryManager.
     */
    void* alloc(uint32_t size) override;

    /**
     * Overriding function from MemoryManager.
     */
    void* alloc(uint32_t size, uint32_t alignment) override;

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
    void free(void* ptr) override;

    /**
     * Overriding function from MemoryManager.
     */
	void free(void* ptr, uint32_t alignment) override;

    /**
     * Dump the list of free chunks of memory.
     */
    void dump() override;
};

#endif
