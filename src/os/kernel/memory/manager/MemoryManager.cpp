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

#include <lib/util/HashMap.h>
#include "MemoryManager.h"

Util::HashMap<String, MemoryManager*> MemoryManager::prototypeTable;

MemoryManager *MemoryManager::createInstance(String type) {
    String key = type.toLowerCase();

    if(prototypeTable.containsKey(key)) {
        return prototypeTable.get(type)->clone();
    }

    Cpu::throwException(Cpu::Exception::UNKNOWN_DRIVER);

    return nullptr;
}

void MemoryManager::registerManagerType(MemoryManager *driver) {
    String key = driver->getName().toLowerCase();

    prototypeTable.put(key, driver);
}

void MemoryManager::deregisterManagerType(String type) {
    if(prototypeTable.containsKey(type)) {
        prototypeTable.remove(type);
    }
}

void MemoryManager::init(uint32_t memoryStartAddress, uint32_t memoryEndAddress, bool doUnmap) {
    this->memoryStartAddress = memoryStartAddress;
    this->memoryEndAddress = memoryEndAddress;
    this->doUnmap = doUnmap;

    freeMemory = memoryEndAddress - memoryStartAddress;
}

void *MemoryManager::alloc(uint32_t size) {
    return nullptr;
}

void *MemoryManager::alloc(uint32_t size, uint32_t alignment) {
    Cpu::throwException(Cpu::Exception::UNSUPPORTED_OPERATION);

    return nullptr;
}

void *MemoryManager::realloc(void *ptr, uint32_t size) {
    Cpu::throwException(Cpu::Exception::UNSUPPORTED_OPERATION);

    return nullptr;
}

void *MemoryManager::realloc(void *ptr, uint32_t size, uint32_t alignment) {
    Cpu::throwException(Cpu::Exception::UNSUPPORTED_OPERATION);

    return nullptr;
}

void MemoryManager::free(void *ptr) {

}

void MemoryManager::free(void *ptr, uint32_t alignment) {
    Cpu::throwException(Cpu::Exception::UNSUPPORTED_OPERATION);
}

void MemoryManager::dump() {
    printf("MemoryManager: dump() not implemented!\n");
}

uint32_t MemoryManager::getStartAddress() {
    return memoryStartAddress;
}

uint32_t MemoryManager::getEndAddress() {
    return memoryEndAddress;
}

uint32_t MemoryManager::getFreeMemory() {
    return freeMemory;
}


