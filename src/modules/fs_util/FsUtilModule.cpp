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
#include "FsUtilModule.h"
#include "RandomNode.h"
#include "ZeroNode.h"

MODULE_PROVIDER {

    return new FsUtilModule();
};

int32_t FsUtilModule::initialize() {

    log = &Logger::get("FILESYSTEM");

    fileSystem = Kernel::getService<FileSystem>();

    fileSystem->createDirectory("/dev/util");

    creatNode("/dev/util", new ZeroNode());
    creatNode("/dev/util", new RandomNode());

    return 0;
}

int32_t FsUtilModule::finalize() {

    return 0;
}

String FsUtilModule::getName() {

    return String();
}

Util::Array<String> FsUtilModule::getDependencies() {

    return Util::Array<String>(0);
}

void FsUtilModule::creatNode(const char *path, VirtualNode *node) {

    if(fileSystem->addVirtualNode(path, node) == FileSystem::ADDING_VIRTUAL_NODE_FAILED) {

        log->error("Unable to create '%s/%s'", path, (const char*) node->getName());
    }
}
