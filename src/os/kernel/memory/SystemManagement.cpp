/** 
 * SystemManagement
 * 
 * Is responsible for everything that has to do with address spaces and memory.
 * Keeps track of all registered address spaces and can dispatch memory requests and
 * mapping requests to the corresponding memory managers and page directories.
 * 
 * @author Burak Akguel, Christian Gesse, Filip Krakowski, Fabian Ruhland, Michael Schoettner
 * @date 2018
 */


#include "SystemManagement.h"

#include <kernel/Bios.h>
#include <kernel/interrupts/IntDispatcher.h>
#include <lib/libc/printf.h>
#include <lib/multiboot/Structure.h>
#include <kernel/log/Logger.h>
#include <devices/Pit.h>
#include <kernel/log/StdOutAppender.h>
#include "kernel/cpu/Cpu.h"
#include "kernel/memory/Paging.h"

// some external functions are implemented in assembler code
extern "C"{
    #include "lib/libc/string.h"
	// load CR3 with physical address of page directory
	void load_page_directory(uint32_t* pdAddress);
	// functions to set up memory management and paging
    void _init();
    void _fini();
    void init_system(Multiboot::Info *address);
    void fini_system();
}

// initialize static members
SystemManagement* SystemManagement::systemManagement = nullptr;
HeapMemoryManager* SystemManagement::kernelMemoryManager = nullptr;
bool SystemManagement::initialized = false;
bool SystemManagement::kernelMode = true;

/**
 * Is called from assembler code before calling the main function, because it sets up
 * everything to get the system run.
 */
void init_system(Multiboot::Info *address) {
	// init code segment for bios calls
    Bios::init();
    // enable interrupts afterwards
    Cpu::enableInterrupts();

    // create an instance of the SystemManagement and initialize it
    // (sets up paging and system management)
    SystemManagement *systemManagement = SystemManagement::getInstance();

    systemManagement->init();

    Multiboot::Structure::parse(address);

    Pit::getInstance()->plugin();

    Logger::initialize();

    Logger::setLevel(Multiboot::Structure::getKernelOption("log_level"));

    Logger::setConsoleLogging(Multiboot::Structure::getKernelOption("splash") == "false");

    if(Multiboot::Structure::getKernelOption("gdb") == "false") {
        systemManagement->writeProtectKernelCode();
    }
}

/**
 * Finishes the system and calls global destructors.
 */
void fini_system() {

    _fini();
}

/**
 * Plugin to register for interrupt handling
 */
void SystemManagement::plugin() {
    IntDispatcher::getInstance().assign(IntDispatcher::pagefault, *this);
}


/**
 * Handle a Page Fault
 */
void SystemManagement::trigger() {
#if DEBUG_PM
    printf("[PAGINGMANAGER] Pagefault occured\n");
    printf("[PAGINGMANAGER] Address %x\n", faultedAddress);
    printf("[PAGINGMANAGER] Flags %x\n", faultFlags);
    printf("[PAGINGMANAGER] Floored 4kb aligned address %x\n", (faultedAddress & 0xFFFFF000));
#endif
    // check if pagefault was caused by illegal page access
    if ((faultFlags & 0x00000001) > 0) {
    	Cpu::throwException(Cpu::Exception::ILLEGEAL_PAGE_ACCESS);
    }

    // Map the faulted Page
    map(faultedAddress, PAGE_PRESENT | PAGE_READ_WRITE);
    // TODO: Check other Faults
}

/**
 * Initialize the SystemManager and the corresponding stuff
 */
void SystemManagement::init() {
    // Init Paging Area Manager -> Manages the virtual addresses of all page tables
    // and directories
    pagingAreaManager = new PagingAreaManager();
    // create a Base Page Directory (used to map the kernel into every process)
    basePageDirectory = new PageDirectory();
    // Init the manager for virtual IO Memory
    ioMemManager = new IOMemoryManager();
    // Physical Page Frame Allocator is initialized to be possible to allocate
    // physical memory (page frames)
    calcTotalPhysicalMemory();
    pageFrameAllocator = new PageFrameAllocator(0, totalPhysMemory);

    // init physical memory allocator
    pageFrameAllocator->init();

#if DEBUG_PM
    printf("[PAGINGMANAGER] 4KB paging is activated \n");
#endif

    // init manager for Page Tables and Directories
    pagingAreaManager->init();

    // to be able to map new pages, a bootstrap address space is created.
    // It uses only the basePageDirectory with mapping for kernel space
    currentAddressSpace = new VirtualAddressSpace(basePageDirectory, nullptr);

    // register Paging Manager to handle Page Faults
    this->plugin();

    // init io-memory afterwards, because pagefault will occur setting up the first list header
    ioMemManager->init();

    // now create the first address space with memory managers for kernel and user space
    VirtualAddressSpace *addressSpace = new VirtualAddressSpace(basePageDirectory);
    VirtualAddressSpace *tmp = currentAddressSpace;
    switchAddressSpace(addressSpace);
    // we can delete the bootstrap address space
    delete tmp;
    // add first address space to list with all address spaces
    addressSpaces = new Util::ArrayList<VirtualAddressSpace*>;
    addressSpaces->add(currentAddressSpace);

    // Initialize global objects afterwards, because now missing pages can be mapped
    _init();

    // the memory management system is fully initialized now
    initialized = true;
}

/**
 * Maps a page into the current page directory at a given virtual address.
 */
void SystemManagement::map(uint32_t virtAddress, uint16_t flags) {
	// allocate a physical page frame where the page should be mapped
    uint32_t physAddress = pageFrameAllocator->alloc();
    // map the page into the directory
    currentAddressSpace->getPageDirectory()->map(physAddress, virtAddress, flags);

#if DEBUG_PM
    printf("[PAGINGMANAGER] Map virtual address %x to phys address %x\n", (virtAddress & 0xFFFFF000), physAddress);
#endif
}

/**
 * Maps a page at a given physical address to a virtual address.
 * The physical address should be allocated right now, since this function does
 * only map it!
 */
void SystemManagement::map(uint32_t virtAddress, uint16_t flags, uint32_t physAddress) {
#if DEBUG_PM
    printf("[PAGINGMANAGER] Map virtual address %x to phys address %x\n", (virtAddress & 0xFFFFF000), physAddress);
#endif
    // map the page into the directory
    currentAddressSpace->getPageDirectory()->map(physAddress, virtAddress, flags);
}

/**
 * Range map function to map a range of virtual addresses into the current Page 
 * Directory .
 */
void SystemManagement::map(uint32_t virtStartAddress, uint32_t virtEndAddress, uint16_t flags){
    // get 4kb-aligned start and end address
    uint32_t alignedStartAddress = virtStartAddress & 0xFFFFF000;
    uint32_t alignedEndAddress = virtEndAddress & 0xFFFFF000;
    alignedEndAddress += (virtEndAddress % PAGESIZE == 0) ? 0 : PAGESIZE;
    // map all pages
    for(uint32_t i = alignedStartAddress; i < alignedEndAddress; i += PAGESIZE){
        map(i, flags);
    }
}

/**
 * Creates Page Table for a non present entry in Page Directory
 */
uint32_t SystemManagement::createPageTable(PageDirectory *dir, uint32_t idx){
    // get some virtual memory for the table
    uint32_t virtAddress = pagingAreaManager->alloc();
    // get physical memory for the table
    uint32_t physAddress = getPhysicalAddress(virtAddress);
    // there must be no mapping from virtual to physical address be done here,
    // because the page is zeroed out after allocation by the PagingAreaManager

#if DEBUG_PM
    printf("[PAGINGMANAGER] Create new page table for index %d\n", idx);
#endif

    // create the table in the page directory
    dir->createTable(idx, physAddress, virtAddress);
    return 0;
}

/**
 * Unmap Page at a given virtual address.
 */
uint32_t SystemManagement::unmap(uint32_t virtAddress){
	// request the pagedirectory to unmap the page
    uint32_t physAddress = currentAddressSpace->getPageDirectory()->unmap(virtAddress);
    if(!physAddress){
#if DEBUG_PM
        printf("[PAGINGMAMNAGER] WARN: Page was not present\n");
#endif
        return 0;
    }

    uint32_t ret = pageFrameAllocator->free(physAddress);

#if DEBUG_PM
    printf("[PAGINGMANAGER] Unmap page with virtual address %x\n", virtAddress);
#endif
    // invalidate entry in TLB
    asm volatile("push %%edx;"
        "movl %0,%%edx;"
        "invlpg (%%edx);" 
        "pop %%edx;"  : : "r"(virtAddress));

    return ret;
}

/**
 * Unmap a range of virtual addresses in current Page Directory
 */
uint32_t SystemManagement::unmap(uint32_t virtStartAddress, uint32_t virtEndAddress) {
    // remark: if given addresses are not aligned on pages, we do not want to unmap 
    // data that could be on the same page before startVirtAddress or behind endVirtAddress

    // get aligned start and end address of the area to be freed
    uint32_t startVAddr = virtStartAddress & 0xFFFFF000;
    startVAddr += ((virtStartAddress % PAGESIZE != 0) ? PAGESIZE : 0);
    // calc start address of the last page we want to unmap
    uint32_t endVAddr = virtEndAddress & 0xFFFFF000;
    endVAddr -= (((virtEndAddress + 1) % PAGESIZE != 0) ? PAGESIZE : 0);

    // check if an unmap is possible (the start and end address have to contain
    // at least one complete page)
    if(endVAddr < virtStartAddress) {
        return 0;
    }
    // amount of pages to be unmapped
    uint32_t pageCnt = (endVAddr - startVAddr) / PAGESIZE + 1;

#if DEBUG_PM
    printf("[PAGINGMANAGER] Unmap range [%x, %x] #page %d\n", startVAddr, endVAddr, pageCnt);
#endif

    // loop through the pages and unmap them
    uint32_t ret = 0;
    uint8_t cnt = 0;
    uint32_t i;
    for(i=0; i < pageCnt; i++) {
        ret = unmap(startVAddr + i*PAGESIZE);

        if(!ret) {
            cnt++;
        } else {
            cnt = 0;
        }
        // if there were three pages after each other already unmapped, we break here
        // this is sort of a workaround because by merging large free memory blocks in memory management
        // it might happen that some parts of the memory are already unmapped
        if(cnt == 3) {
            break;
        }
    }


    return ret;
}

/**
 * Reserve phyiscal memory in page frame allocator. This memory be already allocated.
 */
uint32_t SystemManagement::reservePhysicalMemory(uint32_t startAddress, uint32_t endAddress) {
	return pageFrameAllocator->reserveAddressRange(startAddress, endAddress);
}

/**
 * Maps a physical address into the IO-space of the system, located at the upper
 * end of the virtual memory. The allocated memory is 4kb-aligned, therefore the
 * returned virtual memory address is also 4kb-aligned. If the given physical
 * address is not 4kb-aligned, one has to add a offset to the returned virtual
 * memory address in order to obtain the corresponding virtual address.
 */
IOMemInfo SystemManagement::mapIO(uint32_t physAddress, uint32_t size) {
    // get amount of needed pages
    uint32_t pageCnt = size / PAGESIZE;
    pageCnt += (size % PAGESIZE == 0) ? 0 : 1;

    // calculate the physical address of each page that is mapped into the IO-space. 
    uint32_t* physAddresses = new uint32_t[pageCnt];
    for(uint32_t i = 0; i < pageCnt; i++){
        uint32_t paddr = physAddress + i*PAGESIZE ;
        physAddresses[i] = paddr;
    }

    // build up ioMemInfo with given information
    IOMemInfo ioMemInfo = {0, pageCnt, physAddresses};

    // allocate 4kb-aligned virtual IO-memory
    ioMemInfo = ioMemManager->alloc(ioMemInfo);

    if(ioMemInfo.virtStartAddress == 0){
    	delete physAddresses;
        printf("[PAGINGMANAGER] IO Mapping not possible at physical address %x\n", physAddress);
    }

    return ioMemInfo;
}

/**
 * Maps IO-space for a device and allocates physical memory for it. All 
 * allocations are 4kb-aligned.
 */
IOMemInfo SystemManagement::mapIO(uint32_t size) {
    // get amount of needed pages
    uint32_t pageCnt = size / PAGESIZE;
    pageCnt += (size % PAGESIZE == 0) ? 0 : 1;

    // allocate physical pages (memory) for the mapping
    uint32_t* physAddresses = new uint32_t[pageCnt];
    for(uint32_t i = 0; i < pageCnt; i++){
        // allocate one page frame
        uint32_t physAddr = pageFrameAllocator->alloc();
        // check if no physical pages are available
        if(physAddr == 0) {
            printf("[PAGINGMANAGER] IO Mapping not possible no physical frame available");
            // if there are not enough physical page frames available, free
            // all allocations made before and exit with error
            for(uint32_t j=0; j < i; j++) {
                pageFrameAllocator->free(physAddresses[j]);
            }
            return {0,0,0};
        }
        // add physical address to array
        physAddresses[i] = physAddr;
    }

    // build up ioMemInfo with given information
    IOMemInfo ioMemInfo = {0, pageCnt, physAddresses};

    // allocate 4kb-aligned virtual IO-memory
	ioMemInfo = ioMemManager->alloc(ioMemInfo);

    if(ioMemInfo.virtStartAddress == 0){
        printf("[PAGINGMANAGER] IO Mapping not possible with pageCnt %d\n", pageCnt);
        for(uint32_t i = 0; i < pageCnt; i++){
			pageFrameAllocator->free(physAddresses[i]);
		}
    	delete physAddresses;
		return {0,0,0};
    }

    return ioMemInfo;
}

/**
 * Maps IO-space for a device and tries to allocate coherent physical memory block for it. All
 * allocations are 4kb-aligned. If no coherent phys. memory block with this size is available,
 * it returns after an amount of tries with 0.
 * Maybe some more sophisticated implementation should be used later.
 */
IOMemInfo SystemManagement::mapPhysRangeIO(uint32_t size) {
    // get amount of needed pages
    uint32_t pageCnt = size / PAGESIZE;
    pageCnt += (size % PAGESIZE == 0) ? 0 : 1;
    // max amount of rounds to allocate physical memory
    uint32_t maxProbes = 10;

    // allocate physical pages (memory) for the mapping
    uint32_t* physAddresses = new uint32_t[pageCnt];
    // set last Address to 0 because we check if 0 later and memory is not zeroed
    physAddresses[pageCnt-1] = 0;
    // we give 10 tries to allocate enough coherent physical memory
    for(uint32_t probe = 0; probe < maxProbes; probe++){
    	for(uint32_t i = 0; i < pageCnt; i++){
			// allocate one page frame
			uint32_t physAddr = pageFrameAllocator->alloc();
			// check if no physical pages are available
			if(physAddr == 0) {
				// if there are not enough physical page frames available, free
				// all allocations made before
				for(uint32_t j=0; j < i; j++) {
					pageFrameAllocator->free(physAddresses[j]);
					physAddresses[j] = 0;
				}
				// if this is the last try, exit with 0
				if(probe + 1 == maxProbes){
					printf("[PAGINGMANAGER] IO Mapping not possible - no physical frame available\n");
					return {0,0,0};
				}
				break;
			}
			// check if physical memory is not coherent.
			if(i > 0 && physAddr - PAGESIZE != physAddresses[i-1]) {
				// free allocated pagegframes
				for(uint32_t j=0; j < i; j++) {
					pageFrameAllocator->free(physAddresses[j]);
					physAddresses[j] = 0;
				}
				pageFrameAllocator->free(physAddr);
				// if this is the last try, exit with 0
				if(probe + 1 == maxProbes){
					printf("[PAGINGMANAGER] IO Range Mapping not possible - no coherent physical memory available\n");
					return {0,0,0};
				}
				break;
			}
			// add physical address to array
			physAddresses[i] = physAddr;
		}
    	// if last address is not zero then everything is allocated
    	if(physAddresses[pageCnt-1] != 0){
    		break;
    	}
    }

    // build up ioMemInfo with given information
    IOMemInfo ioMemInfo = {0, pageCnt, physAddresses};

    // allocate 4kb-aligned virtual IO-memory
	ioMemInfo = ioMemManager->alloc(ioMemInfo);

    if(ioMemInfo.virtStartAddress == 0){
        printf("[PAGINGMANAGER] IO Range Mapping not possible with pageCnt %d\n", pageCnt);
        for(uint32_t i = 0; i < pageCnt; i++){
        	pageFrameAllocator->free(physAddresses[i]);
        }
    	delete physAddresses;
        return {0,0,0};
    }

    return ioMemInfo;
}

/**
 * Free the IO-space described by the given IOMemInfo Block
 */
void SystemManagement::freeIO(IOMemInfo memInfo) {
    ioMemManager->free(memInfo);
    delete memInfo.physAddresses;
}

/**
 * Gets the physical address of a given virtual address. The returned 
 * physical address is 4kb-aligned, so sometimes an offset may be calculated
 * in order to get the exact physical address corresponding to the virtual
 * address.
 */
uint32_t SystemManagement::getPhysicalAddress(uint32_t virtAddress) {
    return currentAddressSpace->getPageDirectory()->getPhysicalAddress(virtAddress);
}

/**
 * Checks if the system management is fully initialized.
 */
bool SystemManagement::isInitialized() {
    return initialized;
}

/**
 * Checks whether the system is in kernel mode or not.
 * Is needed to decide which heap memory manager is to be used.
 */
bool SystemManagement::isKernelMode() {
	return kernelMode;
}

/**
 * Returns the faulting address of the last pagefault that occured.
 */
uint32_t SystemManagement::getFaultingAddress() {
    return faultedAddress;
}

/**
 * Calculates the amount of usable, installed physical memory using a bios call.
 */
void SystemManagement::calcTotalPhysicalMemory() {

    // request amount of memory
    BC_params->AX = 0xE801;
    BC_params->CX = 0;
    BC_params->DX = 0;
    Bios::Int(0x15);

    // was there a problem?
    if ( (BC_params->AX & 0xFF) == 0x86 || (BC_params->AX & 0xFF) == 0x80) {
        printf("[SYSTEMMANAGEMENT] Physical memory coukld not be calculated.");
        Cpu::halt ();
    } else {
        // calculaze amopunt of usable physivcal memory
        totalPhysMemory = 1024 * 1024;                     // 1. MB
        totalPhysMemory += (BC_params->CX * 1024);         // 2 - 16MB
        totalPhysMemory += (BC_params->DX * 64 * 1024);    // >16MB
    }

    // if there is more than 3,75GB memory apply a cap
    if(totalPhysMemory > PHYS_MEM_CAP) {
        totalPhysMemory = PHYS_MEM_CAP;
    }

    // We need at least 10MB physical memory to run properly
    if(totalPhysMemory < 10 * 1024 * 1024){
        printf("[MEMORYMANAGEMENT] Kernel Panic: not enough RAM\n");
        Cpu::halt();
    }
    printf("[SYSTEMMANAGEMENT] Total Physical Memory: %dMB\n", totalPhysMemory/(1024*1024));

}

/**
 * Creates a new virtual address space and the required memory managers.
 */
VirtualAddressSpace* SystemManagement::createAddressSpace() {
	VirtualAddressSpace *addressSpace = new VirtualAddressSpace(basePageDirectory);
	// add to the list of address spaces
	addressSpaces->add(addressSpace);

	return addressSpace;
}

/**
 * Switches to a given address space.
 */
void SystemManagement::switchAddressSpace(VirtualAddressSpace *addressSpace) {
	// set current address space
	this->currentAddressSpace = addressSpace;
	// load cr3-register with phys. address of Page Directory
	load_page_directory(addressSpace->getPageDirectory()->getPageDirectoryPhysicalAddress());
}

/**
 * Remove an address space from the system
 */
void SystemManagement::removeAddressSpace(VirtualAddressSpace *addressSpace){
	// the current active address space cannot be removed
	if(currentAddressSpace == addressSpace){
		return;
	}
	// remove from list
	addressSpaces->remove(addressSpace);
	// call destructor
	delete addressSpace;
}

/**
 * Allocates space in PageTableArea.
 */
uint32_t SystemManagement::allocPageTable() {
	return pagingAreaManager->alloc();
}

/**
 * Frees a Page Table / Directory.
 */
void SystemManagement::freePageTable(uint32_t virtTableAddress) {
    uint32_t physAddress = getPhysicalAddress(virtTableAddress);
	// free virtual memory
	pagingAreaManager->free(virtTableAddress);
	// free physical memory
	pageFrameAllocator->free(physAddress);
}

/**
 * Sets the params for a page fault.
 */
void SystemManagement::setFaultParams(uint32_t faultAddress, uint32_t flags) {
	faultedAddress = faultAddress;
	faultFlags = flags;
}

/**
 * Getter method for the singleton-construction.
 * Creates an instance if necessary.
 */
SystemManagement* SystemManagement::getInstance() {
	if(systemManagement == nullptr) {
		// create a static memory manager for the kernel heap
        static HeapMemoryManager heapMemoryManager(PHYS2VIRT(Multiboot::Structure::physReservedMemoryEnd), VIRT_KERNEL_HEAP_END);
        // set the kernel heap memory manager to this manager
		kernelMemoryManager = &heapMemoryManager;
		// initialize the kenrel heap
        kernelMemoryManager->init();
        // use the new memory manager to alloc memory for the instance of SystemManegement
		systemManagement = new SystemManagement();
	}
	return systemManagement;
}


void SystemManagement::writeProtectKernelCode() {
    basePageDirectory->writeProtectKernelCode();
}



/*
 * Now we override the new and delete operators of C++ to use our
 * own memory management. Every operator uses the memory manager for user or kernel
 * mode, regarding to the mode the system is currently running in.
 */

void* operator new ( uint32_t size ) {
	if(!SystemManagement::isKernelMode()){
		return SystemManagement::getInstance()->getCurrentUserSpaceHeapManager()->alloc(size);
	} else {
		return SystemManagement::getKernelHeapManager()->alloc(size);
	}

}

void* operator new[]( uint32_t count ) {
	if(!SystemManagement::isKernelMode()){
		return SystemManagement::getInstance()->getCurrentUserSpaceHeapManager()->alloc(count);
	} else {
		return SystemManagement::getKernelHeapManager()->alloc(count);
	}
}

void operator delete ( void* ptr )  {
	if(!SystemManagement::isKernelMode()){
		return SystemManagement::getInstance()->getCurrentUserSpaceHeapManager()->free(ptr);
	} else {
		return SystemManagement::getKernelHeapManager()->free(ptr);
	}
}

void operator delete[] ( void* ptr ) {
	if(!SystemManagement::isKernelMode()){
		return SystemManagement::getInstance()->getCurrentUserSpaceHeapManager()->free(ptr);
	} else {
		return SystemManagement::getKernelHeapManager()->free(ptr);
	}
}

// Placement new
void *operator new(size_t, void *p) { return p; }
void *operator new[](size_t, void *p) { return p; }
void  operator delete  (void *, void *) { };
void  operator delete[](void *, void *) { };
