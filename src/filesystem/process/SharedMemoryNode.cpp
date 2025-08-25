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

#include "SharedMemoryNode.h"

#include "kernel/service/MemoryService.h"
#include "kernel/service/Service.h"
#include "async/SharedMemory.h"
#include "lib/interface.h"

namespace Filesystem::Process {

SharedMemoryNode::SharedMemoryNode(const Util::String &name, Kernel::SharedMemory &sharedMemory, uint32_t processId) :
    MemoryNode(name), sharedMemory(sharedMemory), processId(processId) {}

Util::Io::File::Type SharedMemoryNode::getType() {
    return Util::Io::File::SYSTEM;
}

bool SharedMemoryNode::control(uint32_t request, const Util::Array<uint32_t> &parameters) {
    switch (request) {
        case Util::Async::SharedMemory::MAP: {
            auto &memoryService = Kernel::Service::getService<Kernel::MemoryService>();
            const auto address = reinterpret_cast<void*>(parameters[0]);

            return memoryService.mapSharedMemory(processId, getName(), address);
        }
    }

    return false;
}

}

