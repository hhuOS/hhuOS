/*
 * Copyright (C) 2018-2025 Heinrich-Heine-Universitaet Duesseldorf,
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
#include "lib/util/base/String.h"
#include "lib/util/collection/Array.h"

namespace Filesystem::Memory {

MemoryWrapperNode::MemoryWrapperNode(MemoryNode &node) : node(node) {}

Util::String MemoryWrapperNode::getName() {
    return node.getName();
}

Util::Io::File::Type MemoryWrapperNode::getType() {
    return node.getType();
}

uint64_t MemoryWrapperNode::getLength() {
    return node.getLength();
}

Util::Array<Util::String> MemoryWrapperNode::getChildren() {
    return node.getChildren();
}

uint64_t MemoryWrapperNode::readData(uint8_t *targetBuffer, uint64_t pos, uint64_t numBytes) {
    return node.readData(targetBuffer, pos, numBytes);
}

uint64_t MemoryWrapperNode::writeData(const uint8_t *sourceBuffer, uint64_t pos, uint64_t numBytes) {
    return node.writeData(sourceBuffer, pos, numBytes);
}

bool MemoryWrapperNode::control(uint32_t request, const Util::Array<uint32_t> &parameters) {
    return node.control(request, parameters);
}

bool MemoryWrapperNode::isReadyToRead() {
    return node.isReadyToRead();
}

}