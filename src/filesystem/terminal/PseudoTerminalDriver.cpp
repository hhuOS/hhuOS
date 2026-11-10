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

#include "PseudoTerminalDriver.h"

#include "PseudoTerminalMasterNode.h"
#include "filesystem/memory/NullNode.h"
#include "filesystem/memory/StreamNode.h"
#include "util/io/stream/PipedInputStream.h"
#include "util/io/stream/PipedOutputStream.h"

namespace Filesystem::Terminal {

PseudoTerminalDriver::PseudoTerminalDriver() {
    addNode("/", new Memory::NullNode("pt-manager"));
}

Node* PseudoTerminalDriver::getNode(const Util::String &path) {
    if (path == "pt-manager") {
        return createPseudoTerminal();
    }

    return MemoryDriver::getNode(path);
}

Node* PseudoTerminalDriver::createPseudoTerminal() {
    const auto masterInput = new Util::Io::PipedInputStream();
    const auto masterOutput = new Util::Io::PipedOutputStream();
    const auto slaveInput = new Util::Io::PipedInputStream();
    const auto slaveOutput = new Util::Io::PipedOutputStream();

    masterOutput->connect(*slaveInput);
    slaveOutput->connect(*masterInput);

    const auto nodeName = Util::String::format("pt%u", pseudoTerminalIdGenerator.getNextId());

    auto *masterNode = new PseudoTerminalMasterNode(*this, nodeName, masterOutput, masterInput);
    auto *slaveNode = new Memory::StreamNode(nodeName, slaveOutput, slaveInput);

    addNode("/", slaveNode);

    return masterNode;
}

}