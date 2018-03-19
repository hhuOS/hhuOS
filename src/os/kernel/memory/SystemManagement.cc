/** 
 * PagingManager
 * 
 * Manages everything that has to do with paging. For example, the basic page
 * directory for kernel mappings is managed here. Furthermore, the Paging Manager
 * has to direct map-requests to the corresponding page directories.  
 * 
 * @author Burak Akguel, Christian Gesse
 * @date 2017
 */


#include "SystemManagement.h"

#include <kernel/Bios.h>
#include <kernel/interrupts/IntDispatcher.h>
#include <lib/libc/printf.h>
#include <lib/multiboot/Structure.h>
#include "kernel/Cpu.h"
#include "kernel/memory/Paging.h"


extern "C"{
    #include "lib/libc/string.h"
	void load_page_directory(uint32_t* pdAddress);
    void _init();
    void _fini();
    void init_system();
    void fini_system();
}

SystemManagement* SystemManagement::memoryManagement = nullptr;
HeapMemoryManager* SystemManagement::kernelMemoryManager = nullptr;
bool SystemManagement::initialized = false;
bool SystemManagement::kernelMode = true;

void init_system() {

    Bios::init();

    Cpu::enableInterrupts();

    SystemManagement::getInstance()->init();
}

void fini_system() {

    _fini();
}

/**
 * Plugin to register for interrupt handling
 * 
 */
void SystemManagement::plugin() {
    IntDispatcher::assign(IntDispatcher::pagefault, *this);
}


/**
 * Handle a Page Fault
 * 
 */
void SystemManagement::trigger() {
#if DEBUG_PM
    printf("[PAGINGMANAGER] Pagefault occured\n");
    printf("[PAGINGMANAGER] Address %x\n", faultedAddress);
    printf("[PAGINGMANAGER] Flags %x\n", faultFlags);
    printf("[PAGINGMANAGER] Floored 4kb aligned address %x\n", (faultedAddress & 0xFFFFF000));
#endif
    // Map the faulted Page
    map(faultedAddress, PAGE_PRESENT | PAGE_READ_WRITE);
    // TODO: Check other Faults
}

/**
 * Initialize the Pagingmanager and the corresponding stuff
 * 
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

    // init io-memory afterwards, because pagefault can occur
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

    // Initialize global objects
    _init();

    // the memory management system is fully initialized now
    initialized = true;
}

/**
 * Maps a page into the current page directory at a given virtual address.
 * 
 * @param virtAddress Virtual address where a page should be mapped
 * @param flags Flags for Page Table Entry
 */
void SystemManagement::map(uint32_t virtAddress, uint16_t flags) {
    uint32_t physAddress = pageFrameAllocator->alloc();
#if DEBUG_PM
    printf("[PAGINGMANAGER] Map virtual address %x to phys address %x\n", (virtAddress & 0xFFFFF000), physAddress);
#endif
    // map the page into the directory
    currentAddressSpace->getPageDirectory()->map(physAddress, virtAddress, flags);
}

/**
 * Maps a page at a given physical address to a virtual address.
 * 
 * @param virtAddress Virtual address where a page should be mapped
 * @param flags Flags for Page Table entry
 * @param physAddress Physical address that should be mapped
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
 * 
 * @param virtStartAddress Virtual start address of the mapping
 * @param virtEndAddress Virtual start address of the mapping
 * @param flags Flags for the Page Table entries
 */
void SystemManagement::map(uint32_t virtStartAddress, uint32_t virtEndAddress, uint16_t flags){
    // get 4kb-aligned start and end address
    uint32_t alignedStartAddress = virtStartAddress & 0xFFFFF000;
    uint32_t alignedEndAddress = virtEndAddress & 0xFFFFF000;
    alignedEndAddress += (virtEndAddress % 0x1000 == 0) ? 0 : PAGESIZE;
    // map all pages
    for(uint32_t i = alignedStartAddress; i < alignedEndAddress; i += PAGESIZE){
        map(i, flags);
    }
}

/**
 * Creates Page Table for a non present entry in Page Directory
 * 
 * @param dir Page Directory where Table should be mapped 
 * @param idx Index into the Page Directory
 * @return uint32_t Return Value (TODO: is it necessary?)
 */
uint32_t SystemManagement::createPageTable(PageDirectory *dir, uint32_t idx){
    // get some virtual memory for the table
    uint32_t virtAddress = pagingAreaManager->alloc();
    // get physical memory for the table
    uint32_t physAddress = getPhysicalAddress(virtAddress);

#if DEBUG_PM
    printf("[PAGINGMANAGER] Create new page table for index %d\n", idx);
#endif
    
    // write 0 to all entries
    memset((void*)virtAddress, 0, PAGESIZE);
    // create the table in the page directory
    dir->createTable(idx, physAddress, virtAddress);
    return 0;
}

/**
 * Unmap Page at a given virtual address.
 * 
 * @param virtAddress Virtual Address to be unmapped
 * @return uint32_t Physical Address of unmapped page
 */
uint32_t SystemManagement::unmap(uint32_t virtAddress){
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

    asm volatile("push %%edx;"
        "movl %0,%%edx;"
        "invlpg (%%edx);" 
        "pop %%edx;"  : : "r"(virtAddress));

    return ret;
}

/**
 * Unmap a range of virtual addresses in current Page Directory
 * 
 * @param startVirtAddress Virtual start address to be unmapped
 * @param endVirtAddress last address to be unmapped
 * @return uint32_t Physical address of the last unmapped page
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

        if(cnt == 3) {
            break;
        }
    }


    return ret;
}

/**
 * Maps a physical address into the IO-space of the system, located at the upper
 * end of the virtual memory. The allocated memory is 4kb-aligned, therefore the
 * returned virtual memory address is also 4kb-aligned. If the given physical
 * address is not 4kb-aligned, one has to add a offset to the returned virtual
 * memory address in order to obtain the corresponding virtual address.
 * 
 * @param physAddr Physical address to be mapped. This address is usually given 
 *                 by a hardware device (e.g. for the LFB) and must not be 
 *                 located in physical memory (physical addresses for device
 *                 memories are located above installed physical memory)  
 * @param size Size of memory to be allocated
 * @return IOMemInfo A struct containing the virtual address of the mapped 
 *                   memory and the corresponding physical addresses 
 */
IOMemInfo SystemManagement::mapIO(uint32_t physAddress, uint32_t size) {
    // get amount of needed pages
    uint32_t pageCnt = size / PAGESIZE;
    pageCnt += (size % PAGESIZE == 0) ? 0 : 1;

    // if the physical address conflicts with installed physical memory,
    // exit here
    if(physAddress <= totalPhysMemory){
        printf("[PAGINGMANAGER] Physical address access denied %x\n", physAddress);
        return (IOMemInfo) {0,0,0};
    }

    // calculate the physical address of each page that is mapped into the IO-space. 
    uint32_t* physAddresses = new uint32_t[pageCnt];
    for(uint32_t i = 0; i < pageCnt; i++){
        uint32_t paddr = physAddress + i*PAGESIZE ;
        physAddresses[i] = paddr;
    }

    // allocate 4kb-aligned virtual IO-memory
    IOMemInfo ioMemInfo = ioMemManager->alloc(physAddresses, pageCnt);
    if(ioMemInfo.virtStartAddress == 0){
        printf("[PAGINGMANAGER] IO Mapping not possible at physical address %x\n", physAddress);
    }

    return ioMemInfo;
}

/**
 * Maps IO-space for a device and allocates physical memory for it. All 
 * allocations are 4kb-aligned.
 * 
 * @param size Size of IO-memory to be allocated
 * @return IOMemInfo A struct containing the virtual address of the mapped 
 *                   memory and the corresponding physical addresses 
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

    // allocate virtual IO-memory 
    IOMemInfo ioMemInfo = ioMemManager->alloc(physAddresses, pageCnt);
    if(ioMemInfo.virtStartAddress == 0){
        printf("[PAGINGMANAGER] IO Mapping not possible with pageCnt %d\n", pageCnt);
        for(uint32_t i = 0; i < pageCnt; i++){
			pageFrameAllocator->free(physAddresses[i]);
		}
		return {0,0,0};
    }

    return ioMemInfo;
}

/**
 * Maps IO-space for a device and tries to allocate coherent physical memory block for it. All
 * allocations are 4kb-aligned. If no coherent phys. memory block with this size is available,
 * it returns after an amount of tries with 0.
 * Maybe some more sophisticated implementation should be used later.
 *
 * @param size Size of IO-memory to be allocated
 * @return IOMemInfo A struct containing the virtual address of the mapped
 *                   memory and the corresponding physical addresses
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

    // allocate virtual IO-memory
    IOMemInfo ioMemInfo = ioMemManager->alloc(physAddresses, pageCnt);
    if(ioMemInfo.virtStartAddress == 0){
        printf("[PAGINGMANAGER] IO Range Mapping not possible with pageCnt %d\n", pageCnt);
        for(uint32_t i = 0; i < pageCnt; i++){
        	pageFrameAllocator->free(physAddresses[i]);
        }
        return {0,0,0};
    }

    return ioMemInfo;
}

/**
 * Gets the physical address of a given virtual address. The returned 
 * physical address is 4kb-aligned, so sometimes an offset may be calculated
 * in order to get the exact physical address corresponding to the virtual
 * address.
 * 
 * @param virtAddr Virtual address 
 * @return uint32_t Physical address of the given virtual address (4kb-aligned) 
 */
uint32_t SystemManagement::getPhysicalAddress(uint32_t virtAddress) {
    return currentAddressSpace->getPageDirectory()->getPhysicalAddress(virtAddress);
}

/**
 * Free the IO-space described by the given IOMemInfo Block
 * 
 * @param memInfo IOMemInfo Block containing all information about the memory
 *                block tobe freed.
 */
void SystemManagement::freeIO(IOMemInfo memInfo) {
    ioMemManager->free(memInfo);
}

bool SystemManagement::isInitialized() {
    return initialized;
}

bool SystemManagement::isKernelMode() {
	return kernelMode;
}

uint32_t SystemManagement::getFaultingAddress() {
    return faultedAddress;
}

void SystemManagement::calcTotalPhysicalMemory() {

    //Speicherausbau abfragen
    BC_params->AX = 0xE801;
    BC_params->CX = 0;
    BC_params->DX = 0;
    Bios::Int(0x15);

    // Problem?
    if ( (BC_params->AX & 0xFF) == 0x86 || (BC_params->AX & 0xFF) == 0x80) {
        printf("Kernel panic: Speicherausbau konnte vom BIOS nicht erfragt werden.");
        Cpu::halt ();
    } else {
        // verfuegbaren Speicher insgesamt berechnen
        totalPhysMemory = 1024 * 1024;                     // 1. MB
        totalPhysMemory += (BC_params->CX * 1024);         // 2 - 16MB
        totalPhysMemory += (BC_params->DX * 64 * 1024);    // >16MB
    }

//    totalPhysMemory = Multiboot::Structure::getTotalMem();

    // if there is more than 3,75GB memory apply a cap
    if(totalPhysMemory > PHYS_MEM_CAP) {
        totalPhysMemory = PHYS_MEM_CAP;
    }

    // We need at least 10MB physical memory to run properly
    // TODO: Bluescreen
    if(totalPhysMemory < 10 * 1024 * 1024){
        printf("[MEMORYMANAGEMENT] Kernel Panic: not enough RAM\n");
        Cpu::halt();
    }
    printf("[MEMORYMANAGEMENT] Total Physical Memory: %dMB\n", totalPhysMemory/(1024*1024));

}

VirtualAddressSpace* SystemManagement::createAddressSpace() {
	VirtualAddressSpace *addressSpace = new VirtualAddressSpace(basePageDirectory);
	addressSpaces->add(addressSpace);

	return addressSpace;
}

void SystemManagement::switchAddressSpace(VirtualAddressSpace *addressSpace) {
	// set current address space
	this->currentAddressSpace = addressSpace;
	// load cr3-register with phys. address of Page Directory
	load_page_directory(addressSpace->getPageDirectory()->getPageDirectoryPhysicalAddress());
}

void SystemManagement::removeAddressSpace(VirtualAddressSpace *addressSpace){
	// the current active address space cannot be removed
	if(currentAddressSpace == addressSpace){
		return;
	}
	addressSpaces->remove(addressSpace);
	delete addressSpace;
}

uint32_t SystemManagement::allocPageTable() {
	return pagingAreaManager->alloc();
}

void SystemManagement::freePageTable(uint32_t virtTableAddress) {
	pagingAreaManager->free(virtTableAddress);
}

void SystemManagement::setFaultParams(uint32_t faultAddress, uint32_t flags) {
	faultedAddress = faultAddress;
	faultFlags = flags;
}

SystemManagement* SystemManagement::getInstance() {
	if(memoryManagement == nullptr) {

        static HeapMemoryManager heapMemoryManager(VIRT_KERNEL_HEAP_START, VIRT_KERNEL_HEAP_END);

		kernelMemoryManager = &heapMemoryManager;

        kernelMemoryManager->init();

		memoryManagement = new SystemManagement();
	}
	return memoryManagement;
}


/*****************************************************************************
* Nachfolgend sind die Operatoren von C++, die wir hier ueberschreiben      *
* und entsprechend 'mm_alloc' und 'mm_free' aufrufen.                       *
*****************************************************************************/
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
