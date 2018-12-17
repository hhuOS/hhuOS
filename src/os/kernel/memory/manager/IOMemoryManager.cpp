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

#include <kernel/memory/MemLayout.h>
#include <lib/libc/printf.h>
#include "IOMemoryManager.h"

#include "../SystemManagement.h"

#include "kernel/memory/Paging.h"

extern "C" {
    #include "lib/libc/string.h"
}

IOMemoryManager::IOMemoryManager() : BitmapMemoryManager(PAGESIZE, false) {
    BitmapMemoryManager::init(VIRT_IO_START, VIRT_IO_END, true);
}

IOMemoryManager::IOMemoryManager(const IOMemoryManager &copy) : IOMemoryManager() {

}

void IOMemoryManager::init(uint32_t memoryStartAddress, uint32_t memoryEndAddress, bool doUnmap) {
    // Do nothing. The IOMemoryManager will always be initialized by the kernel and has hardcoded values.
}

String IOMemoryManager::getTypeName() {
    return TYPE_NAME;
}

void* IOMemoryManager::alloc(uint32_t size){

    lock.acquire();

    void *ret = BitmapMemoryManager::alloc(size);

    if(ret != nullptr) {
        uint32_t pageCount = (size / PAGESIZE) + ((size % PAGESIZE == 0) ? 0 : 1);

        ioMemoryMap.put(ret, pageCount);
    }

    lock.release();

    return ret;
}

void IOMemoryManager::free(void *ptr){

    auto virtualAddress = (uint32_t) ptr;

    if(virtualAddress < memoryStartAddress || virtualAddress >= memoryEndAddress){
        return;
    }

    SystemManagement &systemManagement = SystemManagement::getInstance();

    uint32_t pageCount = ioMemoryMap.get(ptr);

    lock.acquire();

    for(uint32_t i = 0; i < pageCount; i++) {
        BitmapMemoryManager::free((void*) (virtualAddress + i * PAGESIZE));

        systemManagement.unmap(virtualAddress + i * PAGESIZE);
    }

    ioMemoryMap.remove(ptr);

    lock.release();
}
