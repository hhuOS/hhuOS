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

#include <lib/util/memory/Address.h>
#include "BitmapMemoryManager.h"

namespace Kernel {

BitmapMemoryManager::BitmapMemoryManager(uint32_t startAddress, uint32_t endAddress, uint32_t blockSize, bool zeroMemory) : MemoryManager(startAddress, endAddress), blockSize(blockSize), zeroMemory(zeroMemory) {
    bitmap = new Util::Memory::Bitmap((endAddress - startAddress) / blockSize);
    freeMemory = bitmap->getSize() * blockSize;
}

BitmapMemoryManager::~BitmapMemoryManager() {
    delete bitmap;
}

void *BitmapMemoryManager::alloc(uint32_t size) {
    if (size == 0) {
        return nullptr;
    }

    // get count of blocks that corresponds to aligned size
    uint32_t blockCount = (size / blockSize) + ((size % blockSize == 0) ? 0 : 1);

    uint32_t block = bitmap->findAndSet(blockCount);

    if (block == bitmap->getSize()) {
        onError();
        return nullptr;
    }

    freeMemory -= blockSize * blockCount;

    void *address = reinterpret_cast<void *>(memoryStartAddress + block * blockSize);

    if (zeroMemory) {
        Util::Memory::Address<uint32_t>(address).setRange(0, blockCount * blockSize);
    }

    return address;
}

void BitmapMemoryManager::free(void *ptr) {
    uint32_t address = (uint32_t) ptr - memoryStartAddress;

    // check if pointer points to valid memory
    if ((uint32_t) ptr < memoryStartAddress || (uint32_t) ptr >= memoryEndAddress) {
        return;
    }

    // find number of block corresponding to physical address
    auto blockNumber = (uint32_t) (address / blockSize);

    bitmap->unset(blockNumber);

    freeMemory += blockSize;
}

void BitmapMemoryManager::onError() {
    Util::Exception::throwException(Util::Exception::ILLEGAL_STATE, "BitmapMemoryManager: Out of memory!");
}

}