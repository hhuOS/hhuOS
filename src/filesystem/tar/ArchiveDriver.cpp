/*
 * Copyright (C) 2018-2024 Heinrich-Heine-Universitaet Duesseldorf,
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

#include "ArchiveFileNode.h"
#include "ArchiveDirectoryNode.h"

namespace Filesystem {
class Node;
}  // namespace Filesystem

namespace Filesystem::Tar {

ArchiveDriver::ArchiveDriver(Util::Io::Tar::Archive &archive) : archive(archive), fileHeaders(archive.getFileHeaders()) {}

Node *ArchiveDriver::getNode(const Util::String &path) {
    for(const auto &header : fileHeaders) {
        Util::String currentPath = header.filename;

        if(path == currentPath) {
            return new ArchiveFileNode(archive, header);
        }

        if((path == "") || (currentPath.beginsWith(path) && currentPath[path.length()] == '/')) {
            return new ArchiveDirectoryNode(archive, path);
        }
    }

    return nullptr;
}

bool ArchiveDriver::createNode([[maybe_unused]] const Util::String &path, [[maybe_unused]] Util::Io::File::Type type) {
    return false;
}

bool ArchiveDriver::deleteNode([[maybe_unused]] const Util::String &path) {
    return false;
}
}