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

#ifndef __KERNEL_MEMORY_MEMORYMANAGER_H__
#define __KERNEL_MEMORY_MEMORYMANAGER_H__

#include <cstdint>

/**
 * Basic class for every memory manager.
 *
 * @author Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schoettner
 * @date HHU, 2018
 */
class MemoryManager {

protected:
    // start and end address of the memory area to manage
    uint32_t memoryStartAddress;
    uint32_t memoryEndAddress;
    // track amount of free memory
    uint32_t freeMemory = 0;

    bool doUnmap;

public:
    /**
     * Constructor
     *
     * @param memoryStartAddress Startaddress of the memory area to manage
     * @param memoryEndAddress Endaddress of the memory area to manage
     */
    MemoryManager(uint32_t memoryStartAddress, uint32_t memoryEndAddress, bool doUnmap) {
        this->memoryStartAddress = memoryStartAddress;
        this->memoryEndAddress = memoryEndAddress;
        this->freeMemory = memoryEndAddress - memoryStartAddress;
        this->doUnmap = doUnmap;
    }

    virtual void *alloc(uint32_t size) = 0;

    virtual void *alloc(uint32_t size, uint32_t alignment) { return nullptr; };

    virtual void free(void *ptr) = 0;

    virtual void free(void *ptr, uint32_t alignment) {};

    /**
     * Getter for the end address.
     *
     * @return The end address of the memory area to manage
     */
    uint32_t getEndAddress() {
        return memoryEndAddress;
    }

    /**
     * Getter for amount of free memory.
     *
     * @return The amount of free memory
     */
    uint32_t getFreeMemory() {
        return freeMemory;
    }

    /**
     * Getter for the start address.
     *
     * @return The start address of the memory area to manage
     */
    uint32_t getStartAddress() {
        return memoryStartAddress;
    }
};

#endif
