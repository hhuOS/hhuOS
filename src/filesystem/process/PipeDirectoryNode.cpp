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

#include "PipeDirectoryNode.h"

#include <unistd.h>

#include "kernel/process/Process.h"
#include "kernel/service/ProcessService.h"

Filesystem::Process::PipeDirectoryNode::PipeDirectoryNode(uint32_t processId) : processId(processId) {}

Util::String Filesystem::Process::PipeDirectoryNode::getName() {
    return "pipes";
}

Util::Io::File::Type Filesystem::Process::PipeDirectoryNode::getType() {
    return Util::Io::File::DIRECTORY;
}

uint64_t Filesystem::Process::PipeDirectoryNode::getLength() {
    return 0;
}

Util::Array<Util::String> Filesystem::Process::PipeDirectoryNode::getChildren() {
    auto &processService = Kernel::Service::getService<Kernel::ProcessService>();
    auto *process = processService.getProcess(processId);
    if (process == nullptr) {
        return Util::Array<Util::String>();
    }

    const auto &pipes = process->getPipes();
    auto children = Util::Array<Util::String>(pipes.size());
    for (uint32_t i = 0; i < pipes.size(); i++) {
        children[i] = pipes.get(i).getFirst();
    }

    return children;
}

uint64_t Filesystem::Process::PipeDirectoryNode::readData([[maybe_unused]] uint8_t *targetBuffer, [[maybe_unused]] uint64_t pos, [[maybe_unused]] uint64_t numBytes) {
    return 0;
}

uint64_t Filesystem::Process::PipeDirectoryNode::writeData([[maybe_unused]] const uint8_t *sourceBuffer, [[maybe_unused]] uint64_t pos, [[maybe_unused]] uint64_t numBytes) {
    return 0;
}
