/*
 * Copyright (C) 2018-2023 Heinrich-Heine-Universitaet Duesseldorf,
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

#include "BufferNode.h"

#include "lib/util/base/Address.h"
#include "filesystem/memory/MemoryNode.h"

Filesystem::Memory::BufferNode::BufferNode(const Util::String &name, const uint8_t *buffer, uint32_t length) : MemoryNode(name), buffer(buffer), length(length) {}

uint64_t Filesystem::Memory::BufferNode::getLength() {
    return length;
}

uint64_t Filesystem::Memory::BufferNode::readData(uint8_t *targetBuffer, uint64_t pos, uint64_t numBytes) {
    if (pos >= length) {
        return 0;
    }

    if (pos + numBytes > length) {
        numBytes = length - pos;
    }

    auto sourceAddress = Util::Address<uint32_t>(buffer).add(pos);
    auto targetAddress = Util::Address<uint32_t>(targetBuffer);
    targetAddress.copyRange(sourceAddress, numBytes);

    return numBytes;
}
