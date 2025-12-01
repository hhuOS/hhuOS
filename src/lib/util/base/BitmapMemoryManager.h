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

#ifndef HHUOS_LIB_UTIL_BITMAPMEMORYMANAGER_H
#define HHUOS_LIB_UTIL_BITMAPMEMORYMANAGER_H

#include <stdint.h>

#include "util/async/AtomicBitmap.h"
#include "util/base/BlockMemoryManager.h"

namespace Util {

/// A block memory manager based on an `AtomicBitmap` that manages memory in fixed-size blocks.
/// It is thread-safe and lock free, allowing for fast concurrent allocations and deallocations.
class BitmapMemoryManager : public BlockMemoryManager {

public:
    /// Create a new BitmapMemoryManager instance with a given range of memory and block size.
    /// The start address in inclusive and the end address is exclusive.
    /// An end address of 0 actually points to the highest possible address.
    /// The boolean flag `zeroMemory` indicates whether the allocated memory blocks should be zeroed out.
    BitmapMemoryManager(uint8_t *startAddress, uint8_t *endAddress, size_t blockSize, bool zeroMemory = false);

    /// A memory manager should not be copyable, since copies would operate on the same memory.
    BitmapMemoryManager(const BitmapMemoryManager &copy) = delete;

    /// A memory manager should not be copyable, since copies would operate on the same memory.
    BitmapMemoryManager& operator=(const BitmapMemoryManager &other) = delete;

    /// Allocate a block of memory of the size defined by the block size of this manager (given in the constructor).
    /// If no block is available, a panic is fired.
    void* allocateBlock() override;

    /// Free a block of memory that was previously allocated by this memory manager.
    /// If the given pointer is outside the managed memory range, a panic is fired.
    void freeBlock(void *pointer) override;

    /// Return the total amount of memory managed by this memory manager.
    size_t getTotalMemory() const override;

    /// Calculate the amount of free memory left in this memory manager.
    size_t getFreeMemory() const override;

    /// Return the size of a single block managed by this memory manager.
    size_t getBlockSize() const override;

    /// Return the start address of the managed memory range.
    void* getStartAddress() const override;

    /// Return the end address of the managed memory range.
    void* getEndAddress() const override;

    /// Manually mark a block of memory as used or free.
    /// If the given pointer is outside the managed memory range, a panic is fired.
    /// If it does not point to the start of a block, it is rounded down to the nearest block start address.
    void markBlock(const void *pointer, bool used) const;

protected:
    /// Set a range of blocks as used or free.
    /// This method is not intended to be used directly, but rather as convenience for derived classes.
    void setRange(size_t startBlock, size_t blockCount) const;

private:

    uint8_t *startAddress;
    uint8_t *endAddress;

    const uint32_t blockSize;
    const bool zeroMemory = false;

    Async::AtomicBitmap bitmap;
};

}

#endif
