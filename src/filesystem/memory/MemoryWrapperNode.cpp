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

#include "MemoryWrapperNode.h"

#include "filesystem/memory/MemoryNode.h"

namespace Filesystem::Memory {

MemoryWrapperNode::MemoryWrapperNode(MemoryNode &node) : node(node) {}

Util::Memory::String MemoryWrapperNode::getName() {
    return node.getName();
}

Util::File::Type MemoryWrapperNode::getFileType() {
    return node.getFileType();
}

uint64_t MemoryWrapperNode::getLength() {
    return node.getLength();
}

Util::Data::Array<Util::Memory::String> MemoryWrapperNode::getChildren() {
    return node.getChildren();
}

uint64_t MemoryWrapperNode::readData(uint8_t *targetBuffer, uint64_t pos, uint64_t numBytes) {
    return node.readData(targetBuffer, pos, numBytes);
}

uint64_t MemoryWrapperNode::writeData(const uint8_t *sourceBuffer, uint64_t pos, uint64_t numBytes) {
    return node.writeData(sourceBuffer, pos, numBytes);
}

bool MemoryWrapperNode::control(uint32_t request, const Util::Data::Array<uint32_t> &parameters) {
    return node.control(request, parameters);
}

}