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

#include "lib/util/math/Random.h"
#include "lib/util/base/Address.h"
#include "RandomNode.h"
#include "filesystem/memory/MemoryNode.h"

namespace Filesystem::Memory {

RandomNode::RandomNode(const Util::String &name) : MemoryNode(name) {}

Util::Io::File::Type RandomNode::getType() {
    return Util::Io::File::CHARACTER;
}

uint64_t RandomNode::readData(uint8_t *targetBuffer, [[maybe_unused]] uint64_t pos, uint64_t numBytes) {
    auto target = Util::Address(targetBuffer);
    auto random = Util::Math::Random();

    for (uint32_t i = 0; i < numBytes; i++) {
        target.write8(static_cast<uint8_t>(random.nextRandomNumber() * UINT8_MAX), i);
    }

    return numBytes;
}

}