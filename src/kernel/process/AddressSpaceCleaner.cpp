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

#include "AddressSpaceCleaner.h"

#include "kernel/service/MemoryService.h"
#include "kernel/service/ProcessService.h"
#include "kernel/process/Process.h"
#include "kernel/service/Service.h"
#include "kernel/memory/MemoryLayout.h"
#include "lib/util/base/Constants.h"
#include "kernel/process/Scheduler.h"

namespace Kernel {

void AddressSpaceCleaner::run() {
    auto &processService = Service::getService<ProcessService>();
    auto &currentProcess = Service::getService<ProcessService>().getCurrentProcess();
    while (currentProcess.getThreadCount() > 1) {
        processService.getScheduler().yield();
    }

    Service::getService<MemoryService>().unmap(reinterpret_cast<void*>(Kernel::MemoryLayout::KERNEL_END), ((Kernel::MemoryLayout::MEMORY_END - Kernel::MemoryLayout::KERNEL_END) + 1) / Util::PAGESIZE, 0);
    processService.cleanup(&currentProcess);
}

}