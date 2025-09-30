/*
 * Copyright (C) 2017-2025 Heinrich Heine University Düsseldorf,
 * Institute of Computer Science, Department Operating Systems
 * Main developers: Christian Gesse <christian.gesse@hhu.de>, Fabian Ruhland <ruhland@hhu.de>
 * Original development team: Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schöttner
 * This project has been supported by several students.
 * A full list of integrated student theses can be found here: https://github.com/hhuOS/hhuOS/wiki/Student-theses
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

    auto sourceAddress = Util::Address(buffer).add(pos);
    auto targetAddress = Util::Address(targetBuffer);
    targetAddress.copyRange(sourceAddress, numBytes);

    return numBytes;
}

bool Filesystem::Memory::BufferNode::isReadyToRead() {
    return false;
}
