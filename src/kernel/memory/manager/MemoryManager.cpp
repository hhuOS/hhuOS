/*
 * Copyright (C) 2018-2021 Heinrich-Heine-Universitaet Duesseldorf,
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

#include "util/data/HashMap.h"
#include "MemoryManager.h"

namespace Kernel {

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
    Device::Cpu::throwException(Device::Cpu::Exception::UNSUPPORTED_OPERATION,
                        "MemoryManager: Operation 'aligned alloc' is not supported!");

    return nullptr;
}

void *MemoryManager::realloc(void *ptr, uint32_t size) {
    Device::Cpu::throwException(Device::Cpu::Exception::UNSUPPORTED_OPERATION, "MemoryManager: Operation 'realloc' is not supported!");

    return nullptr;
}

void *MemoryManager::realloc(void *ptr, uint32_t size, uint32_t alignment) {
    Device::Cpu::throwException(Device::Cpu::Exception::UNSUPPORTED_OPERATION,
                        "MemoryManager: Operation 'aligned realloc' is not supported!");

    return nullptr;
}

void MemoryManager::free(void *ptr) {

}

void MemoryManager::free(void *ptr, uint32_t alignment) {
    Device::Cpu::throwException(Device::Cpu::Exception::UNSUPPORTED_OPERATION,
                        "MemoryManager: Operation 'aligned free' is not supported!");
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

}
