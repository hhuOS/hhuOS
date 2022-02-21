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

#include "lib/interface.h"
#include "lib/util/system/SystemCall.h"
#include "lib/util/memory/HeapMemoryManager.h"
#include "lib/util/memory/Constants.h"

void *allocateMemory(uint32_t size, uint32_t alignment) {
    auto *manager = reinterpret_cast<Util::Memory::HeapMemoryManager*>(Util::Memory::USER_SPACE_MEMORY_MANAGER_ADDRESS);
    return manager->allocateMemory(size, alignment);
}

void* reallocateMemory(void *pointer, uint32_t size, uint32_t alignment) {
    auto *manager = reinterpret_cast<Util::Memory::HeapMemoryManager*>(Util::Memory::USER_SPACE_MEMORY_MANAGER_ADDRESS);
    return manager->reallocateMemory(pointer, size, alignment);
}

void freeMemory(void *pointer, uint32_t alignment) {
    auto *manager = reinterpret_cast<Util::Memory::HeapMemoryManager*>(Util::Memory::USER_SPACE_MEMORY_MANAGER_ADDRESS);
    manager->freeMemory(pointer, alignment);
}

bool isSystemInitialized() {
    return true;
}

void* mapIO(uint32_t physicalAddress, uint32_t size) {
    return nullptr;
}

void unmap(uint32_t virtualStartAddress, uint32_t virtualEndAddress) {
    Util::System::SystemCall::execute(Util::System::SystemCall::UNMAP, 2, virtualStartAddress, virtualEndAddress);
}

Util::Memory::String getCanonicalPath(const Util::Memory::String &path) {
    return "";
}

bool createFile(const Util::Memory::String &path, Util::File::Type type) {
    return false;
}

bool deleteFile(const Util::Memory::String &path) {
    return false;
}

int32_t openFile(const Util::Memory::String &path) {
    return -1;
}

void closeFile(int32_t fileDescriptor) {}

Util::File::Type getFileType(int32_t fileDescriptor) {
    return Util::File::REGULAR;
}

uint32_t getFileLength(int32_t fileDescriptor) {
    return -1;
}

Util::Memory::String getFileName(int32_t fileDescriptor) {
    return "";
}

Util::Data::Array<Util::Memory::String> getFileChildren(int32_t fileDescriptor) {
    return Util::Data::Array<Util::Memory::String>(0);
}

uint64_t readFile(int32_t fileDescriptor, uint8_t *targetBuffer, uint64_t pos, uint64_t numBytes) {
    return 0;
}

uint64_t writeFile(int32_t fileDescriptor, const uint8_t *sourceBuffer, uint64_t pos, uint64_t length) {
    return 0;
}

void throwError(Util::Exception::Error error, const char *message) {
    asm volatile ("hlt");
    while(true);
}