/*
 * Copyright (C) 2017-2026 Heinrich Heine University Düsseldorf,
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

#include "ProcessDirectoryNode.h"

#include "lib/util/collection/Array.h"

namespace Filesystem::Process {

ProcessDirectoryNode::ProcessDirectoryNode(uint32_t processId) : name(Util::String::format("%u", processId)) {}

Util::String ProcessDirectoryNode::getName() {
    return name;
}

Util::Io::File::Type ProcessDirectoryNode::getType() {
    return Util::Io::File::DIRECTORY;
}

uint64_t ProcessDirectoryNode::getLength() {
    return 0;
}

Util::Array<Util::String> ProcessDirectoryNode::getChildren() {
    return Util::Array<Util::String>({"name", "cwd", "thread_count", "pipes", "shared"});
}

uint64_t ProcessDirectoryNode::readData([[maybe_unused]] uint8_t *targetBuffer, [[maybe_unused]] uint64_t pos, [[maybe_unused]] uint64_t numBytes) {
    return 0;
}

uint64_t ProcessDirectoryNode::writeData([[maybe_unused]] const uint8_t *sourceBuffer, [[maybe_unused]] uint64_t pos, [[maybe_unused]] uint64_t numBytes) {
    return 0;
}

}