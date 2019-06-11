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

#include "lib/memory/MemoryUtil.h"
#include "StaticHeapMemoryManager.h"

namespace Kernel {

StaticHeapMemoryManager::StaticHeapMemoryManager() : MemoryManager() {

}

StaticHeapMemoryManager::StaticHeapMemoryManager(const StaticHeapMemoryManager &copy) : StaticHeapMemoryManager() {

}

void StaticHeapMemoryManager::init(uint32_t memoryStartAddress, uint32_t memoryEndAddress, bool doUnmap) {
    MemoryManager::init(memoryStartAddress, memoryEndAddress, false);

    currentPosition = memoryStartAddress;
}

String StaticHeapMemoryManager::getTypeName() {
    return TYPE_NAME;
}

void *StaticHeapMemoryManager::alloc(uint32_t size) {
    return alloc(size, 0);
}

void *StaticHeapMemoryManager::alloc(uint32_t size, uint32_t alignment) {
    if (freeMemory < size) {
        return nullptr;
    }

    lock.acquire();

    currentPosition = MemoryUtil::alignUp(currentPosition, alignment);

    void *ret = reinterpret_cast<void *>(currentPosition);

    currentPosition += size;
    freeMemory -= size;

    lock.release();

    return ret;
}

void *StaticHeapMemoryManager::realloc(void *ptr, uint32_t size) {
    return realloc(ptr, size, 0);
}

void *StaticHeapMemoryManager::realloc(void *ptr, uint32_t size, uint32_t alignment) {
    void *ret = alloc(size, alignment);

    if (ret == nullptr) {
        return ret;
    }

    memcpy(ret, ptr, size);

    return ret;
}

void StaticHeapMemoryManager::free(void *ptr) {
    free(ptr, 0);
}

void StaticHeapMemoryManager::free(void *ptr, uint32_t alignment) {

}

void StaticHeapMemoryManager::dump() {
    printf("  StaticHeapMemoryManager: Current position = %08x\n", currentPosition);
}

}