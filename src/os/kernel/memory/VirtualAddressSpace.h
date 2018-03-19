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
