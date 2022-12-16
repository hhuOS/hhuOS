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

#include "lib/util/math/Random.h"
#include "lib/util/memory/Address.h"
#include "RandomNode.h"
#include "filesystem/memory/MemoryNode.h"

namespace Filesystem::Memory {

RandomNode::RandomNode(const Util::Memory::String &name) : MemoryNode(name) {}

Util::File::Type RandomNode::getFileType() {
    return Util::File::CHARACTER;
}

uint64_t RandomNode::readData(uint8_t *targetBuffer, uint64_t pos, uint64_t numBytes) {
    auto target = Util::Memory::Address<uint32_t>(targetBuffer);
    auto random = Util::Math::Random();

    for (uint32_t i = 0; i < numBytes; i++) {
        target.setByte(static_cast<uint8_t>(random.nextRandomNumber() * UINT8_MAX), i);
    }

    return numBytes;
}

}