/*
 * Copyright (C) 2018-2022 Heinrich-Heine-Universitaet Duesseldorf,
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

#include "kernel/paging/MemoryLayout.h"
#include "PagingAreaManager.h"
#include "kernel/paging/Paging.h"
#include "lib/util/Exception.h"

namespace Kernel {

PagingAreaManager::PagingAreaManager() :
        BitmapMemoryManager(MemoryLayout::PAGING_AREA.startAddress, MemoryLayout::PAGING_AREA.endAddress, Kernel::Paging::PAGESIZE, true),
        blockPool(BLOCK_POOL_SIZE) {
    // We use already 256 Page Tables for Kernel mappings and one Page Directory as the Kernel´s PD
    setRange(0, 8 * 32 + 2);
    refillPool();
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