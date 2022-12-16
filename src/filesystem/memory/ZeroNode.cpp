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

#include "ZeroNode.h"

#include "lib/util/memory/Address.h"
#include "filesystem/memory/MemoryNode.h"

namespace Filesystem::Memory {

ZeroNode::ZeroNode(const Util::Memory::String &name) : MemoryNode(name) {}

Util::File::Type ZeroNode::getFileType() {
    return Util::File::CHARACTER;
}

uint64_t ZeroNode::readData(uint8_t *targetBuffer, uint64_t pos, uint64_t numBytes) {
    Util::Memory::Address<uint32_t>(targetBuffer).setRange(0, numBytes);
    return numBytes;
}

}