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

#include <kernel/Kernel.h>
#include "FsMemoryModule.h"
#include "KernelHeapNode.h"
#include "IOMemoryNode.h"
#include "PhysicalMemoryNode.h"
#include "PagingAreaNode.h"

MODULE_PROVIDER {

    return new FsMemoryModule();
};

int32_t FsMemoryModule::initialize() {

    log = &Logger::get("FILESYSTEM");

    fileSystem = Kernel::getService<FileSystem>();

    fileSystem->createDirectory("/dev/memory");

    creatNode("/dev/memory", new KernelHeapNode());
    creatNode("/dev/memory", new IOMemoryNode());
    creatNode("/dev/memory", new PhysicalMemoryNode());
    creatNode("/dev/memory", new PagingAreaNode());

    return 0;
}

int32_t FsMemoryModule::finalize() {

    return 0;
}

String FsMemoryModule::getName() {

    return String();
}

Util::Array<String> FsMemoryModule::getDependencies() {

    return Util::Array<String>(0);
}

void FsMemoryModule::creatNode(const char *path, VirtualNode *node) {

    if(fileSystem->addVirtualNode(path, node) == FileSystem::ADDING_VIRTUAL_NODE_FAILED) {

        log->error("Unable to create '%s/%s'", path, (const char*) node->getName());
    }
}
