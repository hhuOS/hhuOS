/*
 * Copyright (C) 2018-2023 Heinrich-Heine-Universitaet Duesseldorf,
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

#ifndef __PAGEDIRECTORY_H__
#define __PAGEDIRECTORY_H__

#include <cstdint>

namespace Kernel {

/** 
 * PageDirectory
 * 
 * Basic class representing one Page Directory.
 * 
 * @author Christian Gesse, Burak Akguel
 * @date 2017
 * 
 */
class PageDirectory {

public:
    /**
     * Constructor for Base Page Directory. This directory contains kernel mappings and is built manually for bootstrapping.
     */
    PageDirectory();

    /**
     * Constructor for process Page Directories.
     *
     * @param basePageDirectory The Page Directory with kernel mappgins.
     */
    explicit PageDirectory(PageDirectory &basePageDirectory);

    /**
     * Copy Constructor.
     */
    PageDirectory(const PageDirectory &copy) = delete;

    /**
     * Assignment operator.
     */
    PageDirectory &operator=(const PageDirectory &other) = delete;

    /**
     * Destructor.
     */
    ~PageDirectory();

    /**
     * Maps a virtual address to a given physical address with certain flags.
     *
     * @param physicalAddress Physical address to be mapped
     * @param virtualAddress Virtual address to be mapped
     * @param flags Flags for entry in Page Table
     */
    void map(uint32_t physicalAddress, uint32_t virtualAddress, uint16_t flags);

    /**
     * Unmap a given virtual address from this directory.
     *
     * @param virtualAddress Virtual address to be unmapped
     * @return uint32_t Physical address of the memory that was unmapped
     */
    uint32_t unmap(uint32_t virtualAddress);

    /**
     * Get 4 KiB aligned physical address corresponding to the given virtual address.
     *
     * @param virtualAddress Virtual address
     * @return uint32_t Physical address where virtual address is mapped (4KB-aligned)
     */
    void *getPhysicalAddress(void *virtualAddress);

    /**
     * Create a new Page Table in this Page Directory
     *
     * @param index Index of the table in Page Directory
     * @param physicalAddress Physical address of the Table
     * @param virtualAddress Virtual address of the table.
     */
    void createTable(uint32_t index, uint32_t physicalAddress, uint32_t virtualAddress, uint32_t flags);

    /**
     * Protects a given page from unmapping.
     *
     * @param virtualStartAddress Virtual address of the page that should be protected from unmapping.
     */
    void setPageFlags(uint32_t virtualStartAddress, uint32_t flags);

    /**
     * Protect a range of pages from unmapping.
     *
     * @param virtualStartAddress Virtual address of first page to be protected
     * @param virtualStartAddress Virtual address of last page to be protected
     */
    void setPageFlags(uint32_t virtualStartAddress, uint32_t virtualEndAddress, uint32_t flags);

    /**
     * Unprotect a given page from unmapping.
     *
     * @param virtualAddress Virtual address of the page that should be unprotected from unmapping.
     */
    void unsetPageFlags(uint32_t virtualAddress, uint32_t flags);

    /**
     * Unprotect a range of pages from unmapping.
     *
     * @param virtualStartAddress Virtual address of first page to be protected
     * @param virtualStartAddress Virtual address of last page to be protected
     */
    void unsetPageFlags(uint32_t virtualStartAddress, uint32_t virtualEndAddress, uint32_t flags);

    /**
     * Get virtual address of the page directory.
     *
     * @return Virtual address of page directory as pointer.
     */
    uint32_t* getPageDirectoryVirtualAddress() {
        return pageDirectory;
    }

    /**
     * Get physical address of the page directory.
     *
     * @return Physical address of page directory as pointer.
     */
    uint32_t* getPageDirectoryPhysicalAddress() {
        return pageDirectoryPhysicalAddress;
    }

    /**
     * Get pointer to virtual addresses of the page directory.
     *
     * @return Pointer to virtual table addresses
     */
    uint32_t* getVirtualTableAddresses() {
        return virtualTableAddresses;
    }

private:
    // virtual address of page directory
    uint32_t *pageDirectory;
    // physical address of page directory
    uint32_t *pageDirectoryPhysicalAddress;
    // array with virtual page table addresses
    uint32_t *virtualTableAddresses;
};

}

#endif
