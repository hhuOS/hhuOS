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

#include "kernel/multiboot/Structure.h"
#include "PageFrameAllocator.h"

#include "kernel/memory/Paging.h"

namespace Kernel {

PageFrameAllocator::PageFrameAllocator() : BitmapMemoryManager(PAGESIZE, false) {
    managerType = PAGE_FRAME_ALLOCATOR;
}

void PageFrameAllocator::init(uint32_t memoryStartAddress, uint32_t memoryEndAddress, bool doUnmap) {
    BitmapMemoryManager::init(memoryStartAddress, memoryEndAddress, false);

    // read out how much memory is already used by the system and the initrd
    uint32_t maxIndex = (Multiboot::Structure::physReservedMemoryEnd / PAGESIZE + 1024 + 256) / 32;

    // first X MB are already allocated by 4MB paging
    // X MB + 8KB are already used by kernel and page tables/dirs
    bitmap->setRange(0, maxIndex * 32 + 2);

    // subtract already reserved memory from free memory
    freeMemory -= (maxIndex * 32 * blockSize + 2 * blockSize);
}

String PageFrameAllocator::getTypeName() {
    return TYPE_NAME;
}

}