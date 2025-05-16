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

#ifndef __FREELISTMEMORYMANAGER_H__
#define __FREELISTMEMORYMANAGER_H__

#include <stdint.h>

#include "lib/util/async/Spinlock.h"
#include "HeapMemoryManager.h"
#include "lib/util/base/String.h"
#include "lib/util/reflection/Prototype.h"

namespace Util {

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
    /**
     * Constructor.
     */
    FreeListMemoryManager() = default;

    /**
     * Copy Constructor.
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

    PROTOTYPE_IMPLEMENT_CLONE(FreeListMemoryManager);

    PROTOTYPE_IMPLEMENT_GET_CLASS_NAME("Util::FreeListMemoryManager")

    /**
     * Overriding function from HeapMemoryManager.
     */
    void initialize(uint8_t *startAddress, uint8_t *endAddress) override;

    /**
     * Overriding function from HeapMemoryManager.
     */
    [[nodiscard]] void* allocateMemory(uint32_t size, uint32_t alignment) override;

    /**
     * Overriding function from HeapMemoryManager.
     */
    [[nodiscard]] void* reallocateMemory(void *ptr, uint32_t size, uint32_t alignment) override;

    /**
     * Overriding function from HeapMemoryManager.
     */
    void freeMemory(void *ptr, uint32_t alignment) override;

    /**
     * Overriding function from MemoryManager.
     */
    [[nodiscard]] uint32_t getTotalMemory() const override;

    /**
     * Overriding function from MemoryManager.
     */
    [[nodiscard]] uint32_t getFreeMemory() const override;

    /**
     * Overriding function from MemoryManager.
     */
    [[nodiscard]] uint8_t* getStartAddress() const override;

    /**
     * Overriding function from MemoryManager.
     */
    [[nodiscard]] uint8_t* getEndAddress() const override;

    /**
     * Overriding function from MemoryManager.
     */
    [[nodiscard]] bool isLocked() const override;

    void disableAutomaticUnmapping();

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
     * Implementation of the allocation algorithm, that is used in the alignedAlloc-functions.
     *
     * The first-fit algorithm is used to search for a fitting chunk of free memory.
     *
     * @param size Size of the chunk of memory to be allocated
     * @param alignment Alignment, that chunk of memory should have
     * @param startChunk The chunk of free memory from which to start searching for a fitting chunk
     *
     * @return Pointer to the allocated chunk of memory or nullptr if no chunk with the required size is available
     */
    void* allocAlgorithm(uint32_t size, uint32_t alignment, FreeListHeader *startChunk);

    /**
     * Implementation of the free algorithm, that is used in the free-functions.
     *
     * @param ptr Pointer to the chunk of memory to be freed
     */
    void freeAlgorithm(void *ptr);

private:

    /**
     * Find the next chunk of memory with a required size.
     *
     * @param start Pointer to the chunk of memory from where to start the search
     * @param reqSize Minimal size that is required for the chunk
     *
     * @return Header of the free chunk with the required size or nullptr, if none is found
     */
    static FreeListHeader* findNext(FreeListHeader *start, uint32_t reqSize);

    /**
     * Merge a chunk of free memory with it's neighbours, if possible.
     *
     * @param origin Chunk of free memory to be merged
     */
    FreeListHeader* merge(FreeListHeader *origin);

    uint8_t *startAddress{};
    uint8_t *endAddress{};

    Util::Async::Spinlock lock;
    FreeListHeader *firstChunk = nullptr;
    bool unmapFreedMemory = true;

    static const constexpr uint32_t MIN_BLOCK_SIZE = 4;
    static const constexpr uint32_t HEADER_SIZE = sizeof(FreeListHeader);
};

}

#endif
