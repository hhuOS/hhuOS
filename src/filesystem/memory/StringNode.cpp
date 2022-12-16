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

#include "StringNode.h"

#include "lib/util/memory/Address.h"
#include "filesystem/memory/MemoryNode.h"

namespace Filesystem::Memory {

StringNode::StringNode(const Util::Memory::String &name) : MemoryNode(name) {}

uint64_t StringNode::getLength() {
    return getString().length();
}

uint64_t StringNode::readData(uint8_t *targetBuffer, uint64_t pos, uint64_t numBytes) {
    const auto buffer = getString();

    if (pos >= buffer.length()) {
        return 0;
    }

    if (pos + numBytes > buffer.length()) {
        numBytes = (buffer.length() - pos);
    }

    auto sourceAddress = Util::Memory::Address<uint32_t>(static_cast<const char*>(buffer)).add(pos);
    auto targetAddress = Util::Memory::Address<uint32_t>(targetBuffer);
    targetAddress.copyRange(sourceAddress, numBytes);

    return numBytes;
}

}