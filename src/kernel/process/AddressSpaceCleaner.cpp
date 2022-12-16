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

#include "AddressSpaceCleaner.h"

#include "kernel/service/MemoryService.h"
#include "kernel/system/System.h"
#include "kernel/service/ProcessService.h"
#include "kernel/process/Process.h"
#include "kernel/service/SchedulerService.h"

namespace Kernel {

void AddressSpaceCleaner::run() {
    auto &schedulerService = System::getService<SchedulerService>();
    auto &currentProcess = System::getService<ProcessService>().getCurrentProcess();
    while (currentProcess.getThreadCount() > 1) {
        schedulerService.yield();
    }

    System::getService<MemoryService>().unmap(0, 0xbfffffff, 0);
    schedulerService.cleanup(&currentProcess);
}

}