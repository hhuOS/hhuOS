
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

/** 
 * PageDirectory
 * 
 * Basic class representing one Page Directory.
 * 
 * @author Christian Gesse, Burak Akguel
 * @date 2017
 * 
 */

#ifndef __PAGEDIRECTORY_H__
#define __PAGEDIRECTORY_H__

#include <stdint.h>

#define DEBUG_PD 0


class PageDirectory {

 private:
    // virtual address of page directory
    uint32_t *pageDirectory;
    // physical address of page directory
    uint32_t *physPageDirectoryAddress;
    // array with virtual page table addresses
    uint32_t *virtTableAddresses;

public:
    // constructor to init the first PD with kernel-mappings
    PageDirectory();
    // constructor for all process pds -> need access to kernel mappings
    PageDirectory(PageDirectory *basePageDirectory);
    // destructor - should never be called on basePageDirectory!!!!
    ~PageDirectory();


    // map function to map a given virtual address to a given page frame
    void map(uint32_t physAddress, uint32_t virtAddress, uint16_t flags);
    // create a non-present table in the page directory
    void createTable(uint32_t idx, uint32_t physAddress, uint32_t virtAddress);
    // unmap a page
    uint32_t unmap(uint32_t virtAddress);
    // get physical address for a given virtual address
    uint32_t getPhysicalAddress(uint32_t virtAddress);

    void protectPage(uint32_t virtAddress);
    void protectPage(uint32_t virtStartAddress, uint32_t virtEndAddress);

    void unprotectPage(uint32_t virtAddress);
    void unprotectPage(uint32_t virtStartAddress, uint32_t virtEndAddress);

    uint32_t* getPageDirectoryVirtualAddress(){
    	return pageDirectory;
    }

    uint32_t* getPageDirectoryPhysicalAddress(){
    	return physPageDirectoryAddress;
    }

    uint32_t* getVirtTableAddresses() {
    	return virtTableAddresses;
    }
};

 #endif
