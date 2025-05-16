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

#include "MemoryStatusNode.h"

#include "kernel/service/MemoryService.h"
#include "kernel/service/Service.h"

namespace Kernel {

MemoryStatusNode::MemoryStatusNode(const Util::String &name) : StringNode(name) {}

Util::String MemoryStatusNode::formatMemory(uint32_t value) {
    uint32_t result = value / 1024 / 1024;
    uint32_t rest = value - (result * 1024 * 1024);
    uint32_t comma = (rest * 1000) / 1024 / 1024;

    return Util::String::format("%u.%u MiB", result, comma);
}

Util::String MemoryStatusNode::getString() {
    auto memoryStatus = Kernel::Service::getService<Kernel::MemoryService>().getMemoryStatus();
    return "Physical:      " + formatMemory(memoryStatus.freePhysicalMemory) + " / " + formatMemory(memoryStatus.totalPhysicalMemory) + "\n"
            + "Kernel:        " + formatMemory(memoryStatus.freeKernelHeapMemory) + " / " + formatMemory(memoryStatus.totalKernelHeapMemory) + "\n"
            + "Paging Area:   " + formatMemory(memoryStatus.freePagingAreaMemory) + " / " + formatMemory(memoryStatus.totalPagingAreaMemory) + "\n";
}

}