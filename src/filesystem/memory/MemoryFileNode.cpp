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

#include "lib/util/memory/Address.h"
#include "MemoryFileNode.h"
#include "filesystem/memory/MemoryNode.h"

namespace Filesystem::Memory {

MemoryFileNode::MemoryFileNode(const Util::Memory::String &name) : MemoryNode(name) {}

Util::File::Type MemoryFileNode::getFileType() {
    return Util::File::Type::REGULAR;
}

uint64_t MemoryFileNode::getLength() {
    return length;
}

Util::Data::Array<Util::Memory::String> MemoryFileNode::getChildren() {
    return Util::Data::Array<Util::Memory::String>(0);
}

uint64_t MemoryFileNode::readData(uint8_t *targetBuffer, uint64_t pos, uint64_t numBytes) {
    if (pos >= length) {
        return 0;
    }

    if (pos + numBytes > length) {
        numBytes = (length - pos);
    }

    auto sourceAddress = Util::Memory::Address<uint32_t>(data).add(pos);
    auto targetAddress = Util::Memory::Address<uint32_t>(targetBuffer);
    targetAddress.copyRange(sourceAddress, numBytes);

    return numBytes;
}

uint64_t MemoryFileNode::writeData(const uint8_t *sourceBuffer, uint64_t pos, uint64_t numBytes) {
    auto sourceAddress = Util::Memory::Address<uint32_t>(sourceBuffer);

    if(pos + numBytes >= length) {
        auto newLength = pos + numBytes;
        auto *newData = new uint8_t[newLength];
        auto oldAddress = Util::Memory::Address<uint32_t>(data);
        auto newAddress = Util::Memory::Address<uint32_t>(newData);

        newAddress.setRange(0, newLength);
        newAddress.copyRange(oldAddress, length);

        delete data;
        data = newData;
        length = newLength;
    }

    auto targetAddress = Util::Memory::Address<uint32_t>(data).add(pos);
    targetAddress.copyRange(sourceAddress, numBytes);

    return numBytes;
}

}