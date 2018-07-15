/**
 * VirtualAddressSpace - represents a virtual address space with corresponding page directory
 * and memory managers.
 *
 * @author Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schoettner
 * @date HHU, 2018
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
	this->userSpaceHeapManager = new HeapMemoryManager(PAGESIZE, KERNEL_START);
	// init the userspace manager
	this->userSpaceHeapManager->init();
	// this is no bootstrap address space
	bootstrapAddressSpace = false;
}

/**
 * Constructor for the very first address space for bootstrapping reasons.
 * The memory manager for user space is set manually since it does not exist.
 */
VirtualAddressSpace::VirtualAddressSpace(PageDirectory *pageDirectory, HeapMemoryManager *userSpaceHeapManager) {
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
