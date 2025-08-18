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

#include "PipeNode.h"

namespace Filesystem::Process {

PipeNode::PipeNode(const Util::String &name, Kernel::Pipe &pipe) : MemoryNode(name), pipe(pipe) {}

Util::Io::File::Type PipeNode::getType() {
    return Util::Io::File::CHARACTER;
}

uint64_t PipeNode::readData(uint8_t *targetBuffer, [[maybe_unused]] uint64_t pos, uint64_t numBytes) {
    return pipe.read(targetBuffer, 0, numBytes);
}

uint64_t PipeNode::writeData(const uint8_t *sourceBuffer, [[maybe_unused]] uint64_t pos, uint64_t numBytes) {
    pipe.write(sourceBuffer, 0, numBytes);
    return numBytes;
}

}
