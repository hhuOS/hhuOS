/** 
 * PageDirectory
 * 
 * Basic class representing one Page Directory.
 * 
 * @author Christian Gesse, Burak Akguel
 * @date 2017
 * 
 */

#include <lib/libc/printf.h>
#include "kernel/memory/PageDirectory.h"

#include "kernel/memory/Paging.h"
#include "SystemManagement.h"

extern "C" {
    #include "lib/libc/string.h"
    // extern functions from assembler code Paging.asm
    void load_page_directory(uint32_t* pdAddress);
    void enable_4KB_paging();

}


/**
 * Constructor for Base Page Directory. This directory contains kernel mappings
 * and is built manually for bootstrapping.
 */
PageDirectory::PageDirectory(){
	// table with virtual PT-addresses placed after the PD itself
    virtTableAddresses = (uint32_t*) (VIRT_PAGE_MEM_START + 0x101000);
    // zero memory for PageTables and PageDirectrories
    memset((void*)VIRT_PAGE_MEM_START, 0, PAGESIZE*1024);
    // base page directory is located at VIRT_PAGE_MEM_START + 1MB,
    // because the first 1MB is used for Kernel page tables
    pageDirectory = (uint32_t*) (VIRT_PAGE_MEM_START + 0x100000);
    // virtual page directory is mapped to 9MB
    physPageDirectoryAddress = (uint32_t*) 0x900000;
    
    // set up page directory entries for kernel mappings (all pages addressing
    // > 3 GB are located at 8-9MB)
    // calculate virtual addresses for these tables
    uint32_t i = 0;
    for(uint32_t idx = KERNEL_START / (PAGESIZE*1024); idx < 1024; idx++){
        pageDirectory[idx] = (uint32_t) ((0x000800000 + i * PAGESIZE) | PAGE_PRESENT | PAGE_READ_WRITE);
        virtTableAddresses[idx] = VIRT_PAGE_MEM_START + i * PAGESIZE;
        i++; 
    }
    
    // set the entries for the mapping of first 8 MB
    uint32_t idx = KERNEL_START / (PAGESIZE*1024);
    for(uint16_t i = 0; i < 2048; i++) {
        // this is the physical address of the memory belonging to this page
        uint32_t physAddr = i * PAGESIZE;
        // build up entry for page table by hand (Read/Write and Present bit)
        *((uint32_t *) virtTableAddresses[idx] + i) = physAddr | PAGE_READ_WRITE | PAGE_PRESENT;
        // protect kernel code
        if(i < 1024) {
            *((uint32_t *) virtTableAddresses[idx] + i) |= PAGE_PROTECTED;
        }
    }
    
    // create Mappings for 8MB to 9MB + 8 KB - there are the kernel page tables
    // and the page directory
    idx = VIRT_PAGE_MEM_START / (PAGESIZE*1024);
    for(uint32_t i = 0; i < 258; i++) {
        uint32_t physAddr = 0x800000 + i * PAGESIZE;
        *((uint32_t*)virtTableAddresses[idx] + i) = physAddr | PAGE_PRESENT | PAGE_READ_WRITE | PAGE_PROTECTED;
    }

    // Load the Page Directory into cr3 and enable 4kb-paging via assembler
    load_page_directory(physPageDirectoryAddress);
    enable_4KB_paging();

#if DEBUG_PD
    printf("[PAGEDIRECTORY] Created BasePageDirectory\n");
#endif
    

}

/**
 * Constructor for process Page Directories.
 * 
 * @param basePageDirectory The Page Directory with kernel mappgins.
 */
PageDirectory::PageDirectory(PageDirectory *basePageDirectory){
    pageDirectory = (uint32_t*) SystemManagement::getInstance()->allocPageTable();
    virtTableAddresses = (uint32_t*) SystemManagement::getInstance()->allocPageTable();
    if(pageDirectory == 0 || virtTableAddresses == 0){
    	printf("[PAGEDIRECTORY] Error: could not create PageDirectory\n");
    	return;

    }
    physPageDirectoryAddress = (uint32_t*)SystemManagement::getInstance()->getPhysicalAddress((uint32_t)pageDirectory);
    uint32_t* bp_VirtAddress = basePageDirectory->getPageDirectoryVirtualAddress();
    uint32_t* bp_VirtTableAddresses = basePageDirectory->getVirtTableAddresses();

    uint32_t idx = KERNEL_START / (PAGESIZE * 1024);

    for(; idx < 1024; idx++) {
    	pageDirectory[idx] = bp_VirtAddress[idx];
    	virtTableAddresses[idx] = bp_VirtTableAddresses[idx];
    }
}

PageDirectory::~PageDirectory(){
	// unmap the compelete user space and free the page frames
	for(uint32_t addr = 0; addr < 0xC0000000; addr += 4096){
		this->unmap(addr);
	}
	// free Page Tables corresponding to user space (< 3GB)
	uint32_t idx_max = KERNEL_START / (PAGESIZE * 1024);
	for(uint32_t idx = 0; idx < idx_max; idx++) {
		SystemManagement::getInstance()->freePageTable(virtTableAddresses[idx]);
	}
	// free PageDirectory itself and list with virtual table addresses
	SystemManagement::getInstance()->freePageTable((uint32_t) virtTableAddresses);
	SystemManagement::getInstance()->freePageTable((uint32_t) pageDirectory);

}

/**
 * Maps a virtual address to a given physical address with certain flags.
 * 
 * @param phys Physical address to be mapped
 * @param virt Virtual address to be mapped
 * @param flags Flags for entry in Page Table
 */
void PageDirectory::map(uint32_t physAddress, uint32_t virtAddress, uint16_t flags) {

    // use macros to calculate index into page table and directory
    uint32_t pd_idx = GET_PD_IDX(virtAddress);
    uint32_t pt_idx = GET_PT_IDX(virtAddress);

    // if requested page table is not present, create it
    if( (pageDirectory[pd_idx] & PAGE_PRESENT) == 0) {
        SystemManagement::getInstance()->createPageTable(this, pd_idx);
    }

    // check if the requested page is already mapped -> error
    if( (*((uint32_t*)virtTableAddresses[pd_idx] + pt_idx) & PAGE_PRESENT) != 0) {
        // TODO: bluescreen page already mapped

        printf("[PAGEDIRECTORY] ERROR: Address %x", virtAddress);
        printf(" is already mapped\n");

        
        return;
    }
    // create the entry in the corresponding page table
    *((uint32_t*)virtTableAddresses[pd_idx] + pt_idx) = physAddress | flags;

#if DEBUG_PD
    printf("[PAGEDIRECTORY] Mapped virtual address %x to phys address %x\n", (virtAddress & 0xFFFFF000), physAddress);
#endif
    
}

/**
 * Create a non Page Table in this Page Directory
 * 
 * @param idx Index of the table in Page Directory
 * @param physAddress Physical address of the Table
 * @param virtAddress Virtual address of the table.
 */
void PageDirectory::createTable(uint32_t idx, uint32_t physAddress, uint32_t virtAddress){
    // create the directory entry with the physical address of the table
    pageDirectory[idx] = physAddress | PAGE_PRESENT | PAGE_READ_WRITE;
    // keep track of the virtual address of the table
    virtTableAddresses[idx] = virtAddress;
#if DEBUG_PD
    printf("[PAGEDIRECTORY] Created new page table at virtual address %x\n", virtAddress);
#endif
}

/**
 * Unmap a given virtual address from this directory.
 * 
 * @param virtAddress Virtual address to be unmapped
 * @return uint32_t Physical address of the memory that was unmapped
 */
uint32_t PageDirectory::unmap(uint32_t virtAddress){
    // get indices into Page Table and Directory
    uint32_t pd_idx = GET_PD_IDX(virtAddress);
    uint32_t pt_idx = GET_PT_IDX(virtAddress);

    // if requested page table is not present, the page cannot be mapped
    if( (pageDirectory[pd_idx] & PAGE_PRESENT) == 0) {
        return 0;
    }
    // if the page is not mapped, it cannot be unmapped
    if( (*((uint32_t*)virtTableAddresses[pd_idx] + pt_idx) & PAGE_PRESENT) == 0) {
        return 0;
    }

    // calculate corresponding physical Address and set entry to 0
    uint32_t* vTableAddress = (uint32_t*) virtTableAddresses[pd_idx];

    if(vTableAddress[pt_idx] & PAGE_PROTECTED) {
#if DEBUG_PD
    printf("[PAGEDIRECTORY] Unmap not possible - page is protected\n");
#endif
        return 0;
    }

    uint32_t physAddress = (vTableAddress[pt_idx] & 0xFFFFF000);
    vTableAddress[pt_idx] = 0;

    // TODO: unmap PD if pt is empty (counter array?)

    return physAddress; 
}

/**
 * Get 4kb-aligned physical address corresponding to the given virtual address.
 * 
 * @param virtAddress Virtual address
 * @return uint32_t Physical address where virtual address is mapped (4kb-aligned)
 */
uint32_t PageDirectory::getPhysicalAddress(uint32_t virtAddress) {
    // get indices into Page Table and Directory
    uint32_t pd_idx = GET_PD_IDX(virtAddress);
    uint32_t pt_idx = GET_PT_IDX(virtAddress);

    // if requested page table is not present, the page cannot be mapped
    if( (pageDirectory[pd_idx] & PAGE_PRESENT) == 0) {
        return 0;
    }
    // if the page is not mapped, it cannot be unmapped
    if( (*((uint32_t*)virtTableAddresses[pd_idx] + pt_idx) & PAGE_PRESENT) == 0) {
        return 0;
    }

    // calculate corresponding physical Address and set entry to 0
    uint32_t* vTableAddress = (uint32_t*) virtTableAddresses[pd_idx];
    uint32_t physAddress = (vTableAddress[pt_idx] & 0xFFFFF000);

    return physAddress;
}

void PageDirectory::protectPage(uint32_t virtAddress) {
    uint32_t vaddr = virtAddress & 0xFFFFF000;
    // get indices into Page Table and Directory
    uint32_t pd_idx = GET_PD_IDX(vaddr);
    uint32_t pt_idx = GET_PT_IDX(vaddr);

    // if requested page table is not present, the page cannot be mapped
    if( (pageDirectory[pd_idx] & PAGE_PRESENT) == 0) {
#if DEBUG_PD
    printf("[PAGEDIRECTORY] WARN: Page table not present - cannot protect page\n");
#endif
        return;
    }
    // if the page is not mapped, it cannot be unmapped
    if( (*((uint32_t*)virtTableAddresses[pd_idx] + pt_idx) & PAGE_PRESENT) == 0) {
#if DEBUG_PD
    printf("[PAGEDIRECTORY] WARN: Page not present - cannot protect page\n");
#endif
        return;
    }

    // calculate corresponding physical Address and set entry to 0
    uint32_t* vTableAddress = (uint32_t*) virtTableAddresses[pd_idx];
    uint32_t physAddress = (vTableAddress[pt_idx] | PAGE_PROTECTED);
    vTableAddress[pt_idx] = physAddress;
}

void PageDirectory::protectPage(uint32_t virtStartAddress, uint32_t virtEndAddress) {
    uint32_t startAddr = virtStartAddress & 0xFFFFF000;
    uint32_t endAddr = virtEndAddress & 0xFFFFF000;

    uint32_t i=0;
    while(startAddr + i*PAGESIZE < endAddr) {
        protectPage(startAddr + i*PAGESIZE);
        i++;
    }
}

void PageDirectory::unprotectPage(uint32_t virtAddress) {
    uint32_t vaddr = virtAddress & 0xFFFFF000;
    // get indices into Page Table and Directory
    uint32_t pd_idx = GET_PD_IDX(vaddr);
    uint32_t pt_idx = GET_PT_IDX(vaddr);

    // if requested page table is not present, the page cannot be mapped
    if( (pageDirectory[pd_idx] & PAGE_PRESENT) == 0) {
#if DEBUG_PD
        printf("[PAGEDIRECTORY] WARN: Page table not present - cannot protect page\n");
#endif
        return;
    }
    // if the page is not mapped, it cannot be unmapped
    if( (*((uint32_t*)virtTableAddresses[pd_idx] + pt_idx) & PAGE_PRESENT) == 0) {
#if DEBUG_PD
        printf("[PAGEDIRECTORY] WARN: Page not present - cannot protect page\n");
#endif
        return;
    }

    // calculate corresponding physical Address and set entry to 0
    uint32_t* vTableAddress = (uint32_t*) virtTableAddresses[pd_idx];
    uint32_t physAddress = (vTableAddress[pt_idx] & ~PAGE_PROTECTED);
    vTableAddress[pt_idx] = physAddress;
}

void PageDirectory::unprotectPage(uint32_t virtStartAddress, uint32_t virtEndAddress) {
    uint32_t startAddr = virtStartAddress & 0xFFFFF000;
    uint32_t endAddr = virtEndAddress & 0xFFFFF000;

    uint32_t i=0;
    while(startAddr + i*PAGESIZE < endAddr) {
        unprotectPage(startAddr + i*PAGESIZE);
        i++;
    }
}
