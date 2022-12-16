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

#include "VirtualAddressSpace.h"
#include "lib/util/memory/Constants.h"
#include "kernel/paging/PageDirectory.h"
#include "lib/util/memory/FreeListMemoryManager.h"

namespace Util {
namespace Memory {
class HeapMemoryManager;
}  // namespace Memory
}  // namespace Util

namespace Kernel {

VirtualAddressSpace::VirtualAddressSpace(Util::Memory::HeapMemoryManager &kernelHeapMemoryManager) : memoryManager(&kernelHeapMemoryManager), kernelAddressSpace(true) {
    this->pageDirectory = new PageDirectory();
}

VirtualAddressSpace::VirtualAddressSpace(PageDirectory &basePageDirectory) :
        memoryManager(reinterpret_cast<Util::Memory::FreeListMemoryManager*>(Util::Memory::USER_SPACE_MEMORY_MANAGER_ADDRESS)), kernelAddressSpace(false) {
    // Initialize a new memory abstraction through paging
    this->pageDirectory = new PageDirectory(basePageDirectory);
}

VirtualAddressSpace::~VirtualAddressSpace() {
    delete pageDirectory;
}

PageDirectory &VirtualAddressSpace::getPageDirectory() const {
    return *pageDirectory;
}

Util::Memory::HeapMemoryManager &VirtualAddressSpace::getMemoryManager() const {
    return *memoryManager;
}

bool VirtualAddressSpace::isKernelAddressSpace() const {
    return kernelAddressSpace;
}

}