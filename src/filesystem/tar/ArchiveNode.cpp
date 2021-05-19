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

#include "ArchiveNode.h"

namespace Filesystem::Tar {

ArchiveNode::ArchiveNode(Util::File::Tar::Archive &archive, Util::File::Tar::Archive::Header fileHeader) : type(Util::File::REGULAR) {
    auto path = Util::Memory::String(fileHeader.filename);
    if(!path.isEmpty()) {
        Util::Data::Array<Util::Memory::String> tokens = path.split("/");
        name = tokens[tokens.length() - 1];
    }

    size = Util::File::Tar::Archive::calculateFileSize(fileHeader);
    dataAddress = Util::Memory::Address<uint32_t>(archive.getFile(path), size);
}

ArchiveNode::ArchiveNode(Util::File::Tar::Archive &archive, const Util::Memory::String &path) : type(Util::File::DIRECTORY), dataAddress(static_cast<uint32_t>(0), 0) {
    if(path.isEmpty() || path == "/") {
        name = "/";
    } else {
        Util::Data::Array<Util::Memory::String> tokens = path.split("/");
        name = tokens[tokens.length() - 1];
    }

    for(const auto &header : archive.getFileHeaders()) {
        Util::Memory::String fullPath = header.filename;

        if(fullPath.beginsWith(path) && fullPath != path) {
            Util::Memory::String subPath = fullPath.substring(path.length(), fullPath.length());
            Util::Memory::String childName = subPath.split("/")[0];

            if(!children.contains(childName)) {
                children.add(childName);
            }
        }
    }
}

Util::Memory::String ArchiveNode::getName() {
    return name;
}

Util::File::Type ArchiveNode::getFileType() {
    return type;
}

uint64_t ArchiveNode::getLength() {
    return size;
}

Util::Data::Array<Util::Memory::String> ArchiveNode::getChildren() {
    return children.toArray();
}

uint64_t ArchiveNode::readData(uint8_t *targetBuffer, uint64_t pos, uint64_t numBytes) {
    if (dataAddress == 0) {
        return 0;
    }

    if (pos >= size) {
        return 0;
    }

    if (pos + numBytes > size) {
        numBytes = (size - pos);
    }

    auto targetAddress = Util::Memory::Address<uint32_t>(targetBuffer, numBytes);
    targetAddress.copyRange(dataAddress.add(pos), numBytes);

    return numBytes;
}

uint64_t ArchiveNode::writeData(uint8_t *sourceBuffer, uint64_t pos, uint64_t numBytes) {
    return 0;
}

}