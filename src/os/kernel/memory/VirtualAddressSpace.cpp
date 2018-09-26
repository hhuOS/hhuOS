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

#include "kernel/memory/VirtualAddressSpace.h"
#include "kernel/memory/MemLayout.h"
#include "kernel/memory/SystemManagement.h"
#include "kernel/memory/Paging.h"

/**
 * Constructor for an address space.
 */
VirtualAddressSpace::VirtualAddressSpace(PageDirectory *basePageDirectory) {
	// create a new memory abstraction through paging
	this->pageDirectory = new PageDirectory(basePageDirectory);
	// the kernelspace heap manager is static and global for the system
	this->kernelSpaceHeapManager = SystemManagement::getKernelHeapManager();
	// create a new memory manager for userspace
	this->userSpaceHeapManager = new FreeListMemoryManager(KERNEL_START, PAGESIZE, true);
	// this is no bootstrap address space
	bootstrapAddressSpace = false;
}

/**
 * Constructor for the very first address space for bootstrapping reasons.
 * The memory manager for user space is set manually since it does not exist.
 */
VirtualAddressSpace::VirtualAddressSpace(PageDirectory *pageDirectory, FreeListMemoryManager *userSpaceHeapManager) {
	// use the basePageDirectory here since it is not possible to create a new one right now
	this->pageDirectory = pageDirectory;
	// get the global kernel heap memory manager
	this->kernelSpaceHeapManager = SystemManagement::getKernelHeapManager();
	// the userspace memory manager is passed as an parameter
	this->userSpaceHeapManager = userSpaceHeapManager;
	// this is the address space only used for bootstrapping
	bootstrapAddressSpace = true;
}

/**
 * Destructor
 */
VirtualAddressSpace::~VirtualAddressSpace() {
	// only delete things if they were allocated by the constructor
	if(!bootstrapAddressSpace){
		delete pageDirectory;
		delete userSpaceHeapManager;
	}
}
