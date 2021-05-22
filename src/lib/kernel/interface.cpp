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

#include <lib/interface.h>
#include <kernel/core/Management.h>
#include <kernel/core/System.h>
#include <kernel/service/FilesystemService.h>

void *allocateMemory(uint32_t size) {
    return Kernel::Management::getKernelHeapManager()->alloc(size);
}

void* reallocateMemory(void *pointer, uint32_t size) {
    return Kernel::Management::getKernelHeapManager()->realloc(pointer, size);
}

void freeMemory(void *pointer) {
    Kernel::Management::getKernelHeapManager()->free(pointer);
}

void *allocateMemory(uint32_t size, uint32_t alignment) {
    return Kernel::Management::getKernelHeapManager()->allignedAlloc(size, alignment);
}

void freeMemory(void *pointer, uint32_t alignment) {
    Kernel::Management::getKernelHeapManager()->alignedFree(pointer, alignment);
}

Util::Memory::String getCanonicalPath(const Util::Memory::String &path) {
    return Filesystem::Filesystem::getCanonicalPath(path);
}

Util::File::Node* openFile(const Util::Memory::String &path) {
    auto &filesystem = Kernel::System::getService<Kernel::FilesystemService>()->getFilesystem();
    return filesystem.getNode(path);
}

bool createFile(const Util::Memory::String &path, Util::File::Type type) {
    auto &filesystem = Kernel::System::getService<Kernel::FilesystemService>()->getFilesystem();
    if (type == Util::File::REGULAR) {
        return filesystem.createFile(path);
    } else if (type == Util::File::DIRECTORY) {
        return filesystem.createDirectory(path);
    }

    return false;
}

void closeFile(Util::File::Node *node) {
    delete node;
}

void throwError(Util::Exception::Error error, const char *message){
    Device::Cpu::throwException(error, message);
}
