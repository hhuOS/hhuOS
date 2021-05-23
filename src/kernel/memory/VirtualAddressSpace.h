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

#ifndef __VIRTUALADDRESSSPACE__
#define __VIRTUALADDRESSSPACE__

#include <kernel/file/FileDescriptorManager.h>
#include "kernel/memory/manager/FreeListMemoryManager.h"
#include "kernel/memory/PageDirectory.h"

namespace Kernel {

/**
 * VirtualAddressSpace - represents a virtual address space with corresponding page directory
 * and memory managers.
 *
 * @author Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schoettner
 * @date HHU, 2018
 */
class VirtualAddressSpace {
private:

    FileDescriptorManager fileDescriptorManager;
    // pointer to memory managers for userspace and kernel
    HeapMemoryManager *kernelSpaceHeapManager = nullptr;
    HeapMemoryManager *userSpaceHeapManager = nullptr;
    // pointer to page directory
    PageDirectory *pageDirectory = nullptr;
    // the bootstrap address space is the first address space ever created
    // and only for temporary use
    bool bootstrapAddressSpace = false;

    Util::Memory::String managerType = "";
    uint32_t heapAddress = 0;

    bool initialized = false;
public:
    /**
     * Constructor for an address space with a loaded application.
     */
    VirtualAddressSpace(PageDirectory *basePageDirectory, uint32_t heapAddress, const Util::Memory::String &memoryManagerType = "FreeListMemoryManager");

    /**
     * Constructor for an address space without a loaded application.
     * The heap always start at 0x2000.
     */
    explicit VirtualAddressSpace(PageDirectory *basePageDirectory, const Util::Memory::String &memoryManagerType = "FreeListMemoryManager");

    /**
     * Destructor
     */
    ~VirtualAddressSpace();

    void init();

    bool isInitialized() const;

    /**
     * Get the memory manager for kernel
     *
     * @return Pointer to the kernel memory manager
     */
    HeapMemoryManager *getKernelSpaceHeapManager() const {
        return kernelSpaceHeapManager;
    }

    PageDirectory *getPageDirectory() const {
        return pageDirectory;
    }

    /**
     * Get the memory manager for userspace
     *
     * @return Pointer to the userspace memory manager
     */
    HeapMemoryManager *getUserSpaceHeapManager() const {
        return userSpaceHeapManager;
    }

    /**
     * Set the memory manager for userspace
     *
     * @param userSpaceHeapManager Pointer to the userspace memory manager
     */
    void setUserSpaceHeapManager(HeapMemoryManager *userSpaceHeapManager) {
        this->userSpaceHeapManager = userSpaceHeapManager;
    }

    FileDescriptorManager& getFileDescriptorManager();
};

}

#endif
