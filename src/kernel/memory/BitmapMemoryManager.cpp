/*
 * Copyright (C) 2018-2023 Heinrich-Heine-Universitaet Duesseldorf,
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

#include "lib/util/base/Address.h"
#include "BitmapMemoryManager.h"
#include "lib/util/base/Exception.h"

namespace Kernel {

BitmapMemoryManager::BitmapMemoryManager(uint8_t *startAddress, uint8_t *endAddress, uint32_t blockSize, bool zeroMemory) :
        startAddress(startAddress), endAddress(endAddress), freeMemory(endAddress - startAddress + 1),
        blockSize(blockSize), zeroMemory(zeroMemory), bitmap((endAddress - startAddress + 1) / blockSize) {}

void *BitmapMemoryManager::allocateBlock() {
    uint32_t block = bitmap.findAndSet();

    if (block == bitmap.getSize()) {
        handleError();
        return nullptr;
    }

    freeMemory -= blockSize;

    void *address = reinterpret_cast<void *>(startAddress + block * blockSize);

    if (zeroMemory) {
        Util::Address<uint32_t>(address).setRange(0, blockSize);
    }

    return address;
}

void BitmapMemoryManager::freeBlock(void *pointer) {
    // check if pointer points to valid memory
    if (reinterpret_cast<uint8_t*>(pointer) < startAddress || reinterpret_cast<uint8_t*>(pointer) >= endAddress) {
        Util::Exception::throwException(Util::Exception::OUT_OF_BOUNDS, "free: Trying to free memory outside of heap boundaries");
    }

    // find number of block corresponding to physical address
    auto blockNumber = (reinterpret_cast<uint8_t*>(pointer) - startAddress) / blockSize;

    bitmap.unset(blockNumber);
    freeMemory += blockSize;
}

void BitmapMemoryManager::handleError() {
    Util::Exception::throwException(Util::Exception::ILLEGAL_STATE, "BitmapMemoryManager: Out of memory!");
}

void BitmapMemoryManager::setRange(uint32_t startBlock, uint32_t blockCount) {
    for(uint32_t i = 0; i < blockCount; i++) {
        if(startBlock + i >= bitmap.getSize()) {
            return;
        }

        bitmap.set(startBlock + i);
    }

    freeMemory -= blockCount * blockSize;
}

uint32_t BitmapMemoryManager::getTotalMemory() const {
    return endAddress - startAddress + 1;
}

uint32_t BitmapMemoryManager::getFreeMemory() const {
    return freeMemory;
}

uint32_t BitmapMemoryManager::getBlockSize() const {
    return blockSize;
}

uint8_t * BitmapMemoryManager::getStartAddress() const {
    return startAddress;
}

uint8_t * BitmapMemoryManager::getEndAddress() const {
    return endAddress;
}

}