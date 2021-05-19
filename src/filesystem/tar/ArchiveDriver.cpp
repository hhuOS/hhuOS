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

#include "ArchiveDriver.h"
#include "ArchiveNode.h"

namespace Filesystem::Tar {

ArchiveDriver::ArchiveDriver(Util::File::Tar::Archive &archive) : archive(archive) {

}

Util::Memory::String ArchiveDriver::getClassName() {
    return CLASS_NAME;
}

bool ArchiveDriver::mount() {
    return true;
}

Util::File::Node *ArchiveDriver::getNode(const Util::Memory::String &path) {
    for(const auto &header : archive.getFileHeaders()) {
        Util::Memory::String currentPath = header.filename;

        if(path == currentPath) {
            return new ArchiveNode(archive, header);
        }

        if((path == "") || (currentPath.beginsWith(path) && currentPath[path.length()] == '/')) {
            return new ArchiveNode(archive, path);
        }
    }

    return nullptr;
}

bool ArchiveDriver::createNode(const Util::Memory::String &path) {
    return false;
}

bool ArchiveDriver::deleteNode(const Util::Memory::String &path) {
    return false;
}
}