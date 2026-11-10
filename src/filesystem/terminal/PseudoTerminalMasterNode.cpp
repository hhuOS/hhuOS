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

#include "PseudoTerminalMasterNode.h"

#include "kernel/service/MemoryService.h"
#include "kernel/service/Service.h"

namespace Filesystem::Terminal {

PseudoTerminalMasterNode::~PseudoTerminalMasterNode() {
    driver.deleteNode(slavePath);
}

int64_t PseudoTerminalMasterNode::control(const uint32_t, const uint32_t arg0, const uint32_t, const uint32_t) {
    auto **path = reinterpret_cast<char**>(arg0);

    auto &memoryService = Kernel::Service::getService<Kernel::MemoryService>();
    auto *pathMemory = static_cast<char*>(memoryService.allocateUserMemory(slavePath.length() + 1));

    Util::Address(pathMemory).copyString(static_cast<const char*>(slavePath));
    *path = pathMemory;

    return 0;
}

}
