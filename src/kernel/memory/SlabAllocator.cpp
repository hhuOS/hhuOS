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

#include "SlabAllocator.h"

#include "kernel/memory/BitmapMemoryManager.h"

namespace Kernel {

SlabAllocator::SlabAllocator(uint8_t *slabMemory) :
        pool4K(slabMemory, slabMemory + POOL_SIZE - 1, 4 * 1024),
        pool8K(slabMemory + 1 * POOL_SIZE, slabMemory + 2 * POOL_SIZE - 1, 8 * 1024),
        pool16K(slabMemory + 2 * POOL_SIZE, slabMemory + 3 * POOL_SIZE - 1, 16 * 1024),
        pool32K(slabMemory + 3 * POOL_SIZE, slabMemory + 4 * POOL_SIZE - 1, 32 * 1024),
        pool64K(slabMemory + 4 * POOL_SIZE, slabMemory + 5 * POOL_SIZE - 1, 64 * 1024),
        pool128K(slabMemory + 5 * POOL_SIZE, slabMemory + 6 * POOL_SIZE - 1, 128 * 1024) {}

void *SlabAllocator::allocateBlock(uint32_t frameCount) {
    switch (frameCount) {
        case 1:
            return pool4K.allocateBlock();
        case 2:
            return pool8K.allocateBlock();
        case 3:
        case 4:
            return pool16K.allocateBlock();
        case 5:
        case 6:
        case 7:
        case 8:
            return pool32K.allocateBlock();
        case 9:
        case 10:
        case 11:
        case 12:
        case 13:
        case 14:
        case 15:
        case 16:
            return pool64K.allocateBlock();
        case 17:
        case 18:
        case 19:
        case 20:
        case 21:
        case 22:
        case 23:
        case 24:
        case 25:
        case 26:
        case 27:
        case 28:
        case 29:
        case 30:
        case 31:
        case 32:
            return pool128K.allocateBlock();
        default:
            return nullptr;
    }
}

bool SlabAllocator::freeBlock(void *pointer) {
    if (pointer < pool4K.getStartAddress() || pointer > pool128K.getEndAddress()) {
        return false;
    }

    if (pointer >= pool4K.getStartAddress() && pointer <= pool4K.getEndAddress()) {
        pool4K.freeBlock(pointer);
        return true;
    } else if (pointer >= pool8K.getStartAddress() && pointer <= pool8K.getEndAddress()) {
        pool8K.freeBlock(pointer);
        return true;
    } else if (pointer >= pool16K.getStartAddress() && pointer <= pool16K.getEndAddress()) {
        pool16K.freeBlock(pointer);
        return true;
    } else if (pointer >= pool32K.getStartAddress() && pointer <= pool32K.getEndAddress()) {
        pool32K.freeBlock(pointer);
        return true;
    } else if (pointer >= pool64K.getStartAddress() && pointer <= pool64K.getEndAddress()) {
        pool64K.freeBlock(pointer);
        return true;
    } else if (pointer >= pool128K.getStartAddress() && pointer <= pool128K.getEndAddress()) {
        pool128K.freeBlock(pointer);
        return true;
    }

    return false;
}

uint32_t SlabAllocator::getTotalMemory() const {
    return pool4K.getTotalMemory() + pool8K.getTotalMemory() + pool16K.getTotalMemory() + pool32K.getTotalMemory() + pool64K.getTotalMemory() + pool128K.getTotalMemory();
}

uint32_t SlabAllocator::getFreeMemory() const {
    return pool4K.getFreeMemory() + pool8K.getFreeMemory() + pool16K.getFreeMemory() + pool32K.getFreeMemory() + pool64K.getFreeMemory() + pool128K.getFreeMemory();
}

uint8_t *SlabAllocator::getStartAddress() const {
    return pool4K.getStartAddress();
}

uint8_t *SlabAllocator::getEndAddress() const {
    return pool128K.getEndAddress();
}

}