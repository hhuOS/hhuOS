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

#include "kernel/memory/manager/HeapMemoryManager.h"
#include "kernel/memory/PageDirectory.h"

class VirtualAddressSpace {
private:
	HeapMemoryManager *kernelSpaceHeapManager = nullptr;
	HeapMemoryManager *userSpaceHeapManager = nullptr;
	PageDirectory *pageDirectory = nullptr;
	bool bootstrapAddressSpace = false;
public:
	VirtualAddressSpace(PageDirectory *basePageDirectory);

	// first init constructor
	VirtualAddressSpace(PageDirectory *pageDirectory, HeapMemoryManager *userSpaceHeapManager);

	~VirtualAddressSpace();

	HeapMemoryManager* getKernelSpaceHeapManager() const {
		return kernelSpaceHeapManager;
	}

	PageDirectory* getPageDirectory() const {
		return pageDirectory;
	}

	HeapMemoryManager* getUserSpaceHeapManager() const {
		return userSpaceHeapManager;
	}

	void setUserSpaceHeapManager(HeapMemoryManager* userSpaceHeapManager) {
		this->userSpaceHeapManager = userSpaceHeapManager;
	}
};


#endif
