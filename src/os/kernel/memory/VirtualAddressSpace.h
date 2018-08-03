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

#ifndef __VIRTUALADDRESSSPACE__
#define __VIRTUALADDRESSSPACE__

#include "kernel/memory/manager/FreeListMemoryManager.h"
#include "kernel/memory/PageDirectory.h"


/**
 * VirtualAddressSpace - represents a virtual address space with corresponding page directory
 * and memory managers.
 *
 * @author Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schoettner
 * @date HHU, 2018
 */
class VirtualAddressSpace {
private:
	// pointer to memory managers for userspace and kernelspace
	FreeListMemoryManager *kernelSpaceHeapManager = nullptr;
	FreeListMemoryManager *userSpaceHeapManager = nullptr;
	// pointer to page directory
	PageDirectory *pageDirectory = nullptr;
	// the bootstrap address space is the first address space ever created
	// and only for temporary use
	bool bootstrapAddressSpace = false;
public:
	/**
	 * Constructor for an address space.
	 */
    explicit VirtualAddressSpace(PageDirectory *basePageDirectory);

	/**
	 * Constructor for the very first address space for bootstrapping reasons.
	 * The memory manager for user space is set manually since it does not exist.
	 */
	VirtualAddressSpace(PageDirectory *pageDirectory, FreeListMemoryManager *userSpaceHeapManager);

	/**
	 * Destructor
	 */
	~VirtualAddressSpace();

	/**
	 * Get the memory manager for kernelspace
	 *
	 * @return Pointer to the kernelspace memory manager
	 */
	FreeListMemoryManager* getKernelSpaceHeapManager() const {
		return kernelSpaceHeapManager;
	}

	PageDirectory* getPageDirectory() const {
		return pageDirectory;
	}

	/**
	 * Get the memory manager for userspace
	 *
	 * @return Pointer to the userspace memory manager
	 */
	FreeListMemoryManager* getUserSpaceHeapManager() const {
		return userSpaceHeapManager;
	}

	/**
	 * Set the memory manager for userspace
	 *
	 * @param userSpaceHeapManager Pointer to the userspace memory manager
	 */
	void setUserSpaceHeapManager(FreeListMemoryManager* userSpaceHeapManager) {
		this->userSpaceHeapManager = userSpaceHeapManager;
	}
};


#endif
