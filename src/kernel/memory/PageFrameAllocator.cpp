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

#include "kernel/multiboot/Multiboot.h"
#include "kernel/paging/Paging.h"
#include "PageFrameAllocator.h"
#include "kernel/memory/PagingAreaManager.h"
#include "kernel/memory/TableMemoryManager.h"

namespace Kernel {

PageFrameAllocator::PageFrameAllocator(PagingAreaManager &pagingAreaManager, uint32_t startAddress, uint32_t endAddress) :
        TableMemoryManager(pagingAreaManager, startAddress, endAddress, Kernel::Paging::PAGESIZE) {
    auto *blockMap = Multiboot::getBlockMap();

    // Reserve blocks already used by system image and initrd
    for (uint32_t i = 0; blockMap[i].blockCount != 0; i++) {
        const auto &block = blockMap[i];
        uint32_t blockSize = block.initialMap ? Kernel::Paging::PAGESIZE * 1024 : Kernel::Paging::PAGESIZE;
        uint32_t start = block.startAddress;
        uint32_t end = start + block.blockCount * blockSize - 1;

        setMemory(start, end, 1, block.type == Multiboot::MULTIBOOT_RESERVED);
    }
}

}