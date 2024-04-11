/*
 * Copyright (C) 2018-2024 Heinrich-Heine-Universitaet Duesseldorf,
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

#include "MemoryLayout.h"
#include "PagingAreaManager.h"
#include "Paging.h"
#include "lib/util/base/Exception.h"
#include "lib/util/base/Constants.h"

namespace Kernel {

PagingAreaManager::PagingAreaManager(uint8_t *startAddress, uint32_t mappedPages, uint32_t bootstrapPageCount) : BitmapMemoryManager(startAddress, (startAddress + MemoryLayout::PAGING_AREA_SIZE - 1), Util::PAGESIZE, true), blockPool(BLOCK_POOL_SIZE) {
    setRange(0, bootstrapPageCount);
    for (uint32_t i = 0; i < mappedPages - bootstrapPageCount; i++) {
        void *block = BitmapMemoryManager::allocateBlock();
        blockPool.push(block);
    }
}

void PagingAreaManager::handleError() {
    Util::Exception::throwException(Util::Exception::OUT_OF_PAGING_MEMORY);
}

void *PagingAreaManager::allocateBlock() {
    return blockPool.pop();
}

void PagingAreaManager::freeBlock(void *pointer) {
    BitmapMemoryManager::freeBlock(pointer);
}

void PagingAreaManager::refillPool() {
    if (blockPool.isFull()) {
        return;
    }

    for (uint32_t i = 0; i < blockPool.getCapacity(); i++) {
        void *block = BitmapMemoryManager::allocateBlock();
        if (!blockPool.push(block)) {
            BitmapMemoryManager::freeBlock(block);
            return;
        }
    }
}

}