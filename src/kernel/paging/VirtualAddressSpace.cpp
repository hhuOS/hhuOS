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

#include <lib/util/memory/operators.h>
#include <lib/util/reflection/InstanceFactory.h>
#include "MemoryLayout.h"
#include "Paging.h"
#include "VirtualAddressSpace.h"

namespace Kernel {

VirtualAddressSpace::VirtualAddressSpace(HeapMemoryManager &kernelHeapMemoryManager) :
        memoryManager(&kernelHeapMemoryManager), managerType("FreeListMemoryManager"), heapAddress(0), kernelAddressSpace(true) {
    this->pageDirectory = new PageDirectory();
}

VirtualAddressSpace::VirtualAddressSpace(PageDirectory &basePageDirectory, const Util::Memory::String &memoryManagerType) :
        managerType(memoryManagerType), heapAddress(2 * Kernel::Paging::PAGESIZE), kernelAddressSpace(false) {
    // Initialize a new memory abstraction through paging
    this->pageDirectory = new PageDirectory(basePageDirectory);
}

VirtualAddressSpace::~VirtualAddressSpace() {
    delete pageDirectory;

    if (!kernelAddressSpace) {
        delete memoryManager;
    }
}

void VirtualAddressSpace::initialize() {
    if (!kernelAddressSpace) {
        // Initialize a new memory manager for userspace
        memoryManager = (HeapMemoryManager*) Util::Reflection::InstanceFactory::createInstance(managerType);
        memoryManager->initialize(Util::Memory::Address(heapAddress).alignUp(Kernel::Paging::PAGESIZE).get(), Kernel::MemoryLayout::KERNEL_START - Kernel::Paging::PAGESIZE);
    }

    initialized = true;
}

bool VirtualAddressSpace::isInitialized() const {
    return initialized;
}

PageDirectory &VirtualAddressSpace::getPageDirectory() const {
    return *pageDirectory;
}

HeapMemoryManager &VirtualAddressSpace::getMemoryManager() const {
    return *memoryManager;
}

}