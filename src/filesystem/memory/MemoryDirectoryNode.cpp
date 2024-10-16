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

#include "MemoryDirectoryNode.h"

#include "filesystem/memory/MemoryNode.h"
#include "lib/util/base/Exception.h"

namespace Filesystem::Memory {

MemoryDirectoryNode::MemoryDirectoryNode(const Util::String &name) : MemoryNode(name) {}

Util::Io::File::Type MemoryDirectoryNode::getType() {
    return Util::Io::File::DIRECTORY;
}

uint64_t MemoryDirectoryNode::getLength() {
    return 0;
}

Util::Array<Util::String> MemoryDirectoryNode::getChildren() {
    Util::Array<Util::String> ret(children.size());
    for(uint32_t i = 0; i < children.size(); i++) {
        ret[i] = children.get(i)->getName();
    }

    return ret;
}

uint64_t MemoryDirectoryNode::readData([[maybe_unused]] uint8_t *targetBuffer, [[maybe_unused]] uint64_t pos, [[maybe_unused]] uint64_t numBytes) {
    Util::Exception::throwException(Util::Exception::UNSUPPORTED_OPERATION, "MemoryDriver: Trying to read from a directory!");
}

uint64_t MemoryDirectoryNode::writeData([[maybe_unused]] const uint8_t *sourceBuffer, [[maybe_unused]] uint64_t pos, [[maybe_unused]] uint64_t numBytes) {
    Util::Exception::throwException(Util::Exception::UNSUPPORTED_OPERATION, "MemoryDriver: Trying to write to a directory!");
}

MemoryNode *MemoryDirectoryNode::getChildByName(const Util::String &childName) {
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