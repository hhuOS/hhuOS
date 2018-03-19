#include "kernel/memory/VirtualAddressSpace.h"
#include "kernel/memory/MemLayout.h"
#include "kernel/memory/SystemManagement.h"
#include "kernel/memory/Paging.h"

VirtualAddressSpace::VirtualAddressSpace(PageDirectory *basePageDirectory) {
	this->pageDirectory = new PageDirectory(basePageDirectory);
	this->kernelSpaceHeapManager = SystemManagement::getKernelHeapManager();
	this->userSpaceHeapManager = new HeapMemoryManager(PAGESIZE, KERNEL_START);
	this->userSpaceHeapManager->init();
	bootstrapAddressSpace = false;
}

// first init constructor
VirtualAddressSpace::VirtualAddressSpace(PageDirectory *pageDirectory, HeapMemoryManager *userSpaceHeapManager) {
	this->pageDirectory = pageDirectory;
	this->kernelSpaceHeapManager = SystemManagement::getKernelHeapManager();
	this->userSpaceHeapManager = userSpaceHeapManager;
	bootstrapAddressSpace = true;
}

VirtualAddressSpace::~VirtualAddressSpace() {
	if(!bootstrapAddressSpace){
		delete pageDirectory;
		delete userSpaceHeapManager;
	}
}
