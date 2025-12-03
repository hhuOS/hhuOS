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

#include "BitmapMemoryManager.h"

#include "lib/interface.h"
#include "lib/util/base/Address.h"
#include "lib/util/base/Panic.h"

namespace Util {

void *BitmapMemoryManager::allocateBlock() {
    const auto block = bitmap.findAndSet();
    if (block == Async::AtomicBitmap::INVALID_INDEX) {
        Panic::fire(Panic::OUT_OF_MEMORY, "BitmapMemoryManager: No free memory blocks available");
    }

    void *address = startAddress + block * blockSize;

    if (zeroMemory) {
        Address(address).setRange(0, blockSize);
    }

    return address;
}

void BitmapMemoryManager::freeBlock(void *pointer) {
    // Check if pointer is valid, or else `blockNumber` calculation is undefined behavior.
    if (pointer < startAddress) {
        Util::Panic::fire(Panic::OUT_OF_BOUNDS,
            "BitmapMemoryManager: Trying to free memory outside of heap boundaries");
    }

    // Find number of block corresponding to physical address
    const auto blockNumber = (static_cast<uint8_t*>(pointer) - startAddress) / blockSize;

    // Free the block by unsetting the corresponding bit in the bitmap
    bitmap.unset(blockNumber);
}

size_t BitmapMemoryManager::getFreeMemory() const {
    size_t freeMemory = 0;
    for (size_t i = 0; i < bitmap.getSize(); i++) {
        if (!bitmap.check(i)) {
            freeMemory += blockSize;
        }
    }

    return freeMemory;
}

void BitmapMemoryManager::markBlock(const void *pointer, const bool used) const {
    // Check if pointer is valid, or else `blockNumber` calculation is undefined behavior.
    if (pointer < startAddress) {
        Util::Panic::fire(Panic::OUT_OF_BOUNDS,
            "BitmapMemoryManager: Trying to mark memory outside of heap boundaries");
    }

    // Find number of block corresponding to physical address
    const auto blockNumber = (static_cast<const uint8_t*>(pointer) - startAddress) / blockSize;

    if (used) {
        bitmap.set(blockNumber);
    } else {
        bitmap.unset(blockNumber);
    }
}

void BitmapMemoryManager::setRange(const size_t startBlock, const size_t blockCount) const {
    for(size_t i = 0; i < blockCount; i++) {
        if (startBlock + i >= bitmap.getSize()) {
            return;
        }

        bitmap.set(startBlock + i);
    }
}

}
