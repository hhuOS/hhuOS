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

#include <util/memory/Address.h>
#include "kernel/core/System.h"
#include "BitmapMemoryManager.h"
#include "kernel/memory/Paging.h"

namespace Kernel {

BitmapMemoryManager::BitmapMemoryManager(uint32_t blockSize, bool zeroMemory) : MemoryManager(),
                                                                                blockSize(blockSize),
                                                                                zeroMemory(zeroMemory) {

}

BitmapMemoryManager::BitmapMemoryManager(const BitmapMemoryManager &copy) : BitmapMemoryManager() {

}

BitmapMemoryManager::~BitmapMemoryManager() {
    delete bitmap;
}

void BitmapMemoryManager::init(uint32_t memoryStartAddress, uint32_t memoryEndAddress, bool doUnmap) {
    MemoryManager::init(memoryStartAddress, memoryEndAddress, doUnmap);

    bitmap = new Util::Memory::Bitmap((memoryEndAddress - memoryStartAddress) / blockSize);

    freeMemory = bitmap->getSize() * blockSize;
}

Util::Memory::String BitmapMemoryManager::getTypeName() {
    return TYPE_NAME;
}

void *BitmapMemoryManager::alloc(uint32_t size) {
    if (size == 0) {
        return nullptr;
    }

    // get count of blocks that corresponds to aligned size
    uint32_t blockCount = (size / blockSize) + ((size % blockSize == 0) ? 0 : 1);

    uint32_t block = bitmap->findAndSet(blockCount);

    if (block == bitmap->getSize()) {
        // handle errors
        if (managerType == PAGING_AREA_MANAGER) {
            Device::Cpu::throwException(Device::Cpu::Exception::OUT_OF_PAGE_MEMORY);
        }

        if (managerType == PAGE_FRAME_ALLOCATOR) {
            Device::Cpu::throwException(Device::Cpu::Exception::OUT_OF_PHYS_MEMORY);
        }

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

}