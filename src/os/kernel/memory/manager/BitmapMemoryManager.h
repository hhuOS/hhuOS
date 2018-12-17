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

#ifndef __BITMAPMEMORYMANAGER_H__
#define __BITMAPMEMORYMANAGER_H__

#include "MemoryManager.h"
#include <lib/String.h>

/**
 * Memory manager, that manages a given area of memory in blocks of given size using a bitmap mechanism.
 *
 * This memory manager does not allow allocation with an alignment.
 *
 * TODO: Implement realloc.
 *
 * @author Burak Akguel, Christian Gesse, Filip Krakowski, Fabian Ruhland, Michael Schoettner
 * @date 2018
 */
class BitmapMemoryManager : public MemoryManager{

public:

    enum ManagerType {
        PAGE_FRAME_ALLOCATOR,
        PAGING_AREA_MANAGER,
        MISC
    };

protected:

	/**
	 * The type of this memory manager.
	 */
    ManagerType managerType = MISC;

    /**
     * Bitmap array, which indicates, whether a chunk of memory is free, nor not.
     */
    uint32_t *freeBitmap;

    /**
     * The length of the bitmap array in bytes.
     */
    uint32_t freeBitmapLength;

    /**
     * The size of a single chunk of memory.
     */
    uint32_t blockSize;

    /**
     * The index at which to start searching for free blocks.
     * This allows to block a given number of chunks at the beginning of the managed memory area.
     */
    uint32_t bmpSearchOffset;

    /**
     * Indicates, whether or not allocated memory shall be zero-initialized.
     * This is needed for page tables, that get allocated by a bitmap memory manager.
     */
    bool zeroMemory = false;

private:

    static const constexpr char *TYPE_NAME = "BitmapMemoryManager";

public:

    PROTOTYPE_IMPLEMENT_CLONE(BitmapMemoryManager);

    /**
     * Overriding function from MemoryManager.
     */
    String getTypeName() override;

    /**
     * Constructor.
     */
    explicit BitmapMemoryManager(uint32_t blockSize = 128, bool zeroMemory = false);

    /**
     * Copy-constructor.
     */
    BitmapMemoryManager(const BitmapMemoryManager &copy);

    /**
     * Destructor.
     */
    ~BitmapMemoryManager() override;

    /**
     * Overriding function from MemoryManager.
     */
    void init(uint32_t memoryStartAddress, uint32_t memoryEndAddress, bool doUnmap) override;

    /**
     * Overriding function from memory manager.
     */
    void *alloc(uint32_t size) override;

    /**
     * Overriding function from memory manager.
     */
    void free(void *ptr) override;

    /**
     * Dump the bitmap.
     */
    void dump() override;
};


#endif
