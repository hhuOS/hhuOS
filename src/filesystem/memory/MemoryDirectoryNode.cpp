/*
 * Copyright (C) 2018-2022 Heinrich-Heine-Universitaet Duesseldorf,
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

#include "MemoryDirectoryNode.h"

#include "filesystem/memory/MemoryNode.h"
#include "lib/util/Exception.h"

namespace Filesystem::Memory {

MemoryDirectoryNode::MemoryDirectoryNode(const Util::Memory::String &name) : MemoryNode(name) {}

Util::File::Type MemoryDirectoryNode::getFileType() {
    return Util::File::Type::DIRECTORY;
}

uint64_t MemoryDirectoryNode::getLength() {
    return 0;
}

Util::Data::Array<Util::Memory::String> MemoryDirectoryNode::getChildren() {
    Util::Data::Array<Util::Memory::String> ret(children.size());
    for(uint32_t i = 0; i < children.size(); i++) {
        ret[i] = children.get(i)->getName();
    }

    return ret;
}

uint64_t MemoryDirectoryNode::readData(uint8_t *targetBuffer, uint64_t pos, uint64_t numBytes) {
    Util::Exception::throwException(Util::Exception::UNSUPPORTED_OPERATION, "MemoryDriver: Trying to read from a directory!");
}

uint64_t MemoryDirectoryNode::writeData(const uint8_t *sourceBuffer, uint64_t pos, uint64_t numBytes) {
    Util::Exception::throwException(Util::Exception::UNSUPPORTED_OPERATION, "MemoryDriver: Trying to write to a directory!");
}

MemoryNode *MemoryDirectoryNode::getChildByName(const Util::Memory::String &childName) {
    for(uint32_t i = 0; i < children.size(); i++) {
        if(children.get(i)->getName() == childName) {
            return children.get(i);
        }
    }

    return nullptr;
}

void MemoryDirectoryNode::addChild(MemoryNode *node) {
    children.add(node);
}

}