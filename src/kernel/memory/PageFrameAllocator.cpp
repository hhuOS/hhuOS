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

#include "kernel/multiboot/Multiboot.h"
#include "Paging.h"
#include "PageFrameAllocator.h"
#include "kernel/memory/PagingAreaManager.h"
#include "kernel/memory/TableMemoryManager.h"
#include "device/bus/isa/Isa.h"

extern const uint32_t ___KERNEL_DATA_END__;
const uint32_t KERNEL_DATA_END = reinterpret_cast<uint32_t>(&___KERNEL_DATA_END__);

namespace Kernel {

PageFrameAllocator::PageFrameAllocator(PagingAreaManager &pagingAreaManager, uint8_t *startAddress, uint8_t *endAddress) : TableMemoryManager(pagingAreaManager, startAddress, endAddress, Kernel::Paging::PAGESIZE) {}

void* PageFrameAllocator::allocateBlock() {
    // Try to allocate memory over 16 MiB, to leave free memory for ISA DMA transfers
    auto *ret = TableMemoryManager::allocateBlockAfterAddress(reinterpret_cast<void*>(Device::Isa::MAX_DMA_ADDRESS));
    if (ret == nullptr) {
        // Allocating over 16 MiB failed -> Try to allocate memory after kernel (starts at 1 MiB)
        ret = TableMemoryManager::allocateBlockAfterAddress(reinterpret_cast<void*>(KERNEL_DATA_END));

        if (ret == nullptr) {
            // Allocating after kernel also failed -> As a last resort, we try to allocate below the kernel (< 1 MiB).
            // If this happens during the boot process, things might get screwed up, because the BIOS memory may not yet be reserved.
            // However, in this case we probably have to few memory to even finish booting, so it does not really matter.
            ret = TableMemoryManager::allocateBlock();
        }
    }

    return ret;
}

}