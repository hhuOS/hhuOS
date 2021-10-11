/*
 * Copyright (C) 2018-2021 Heinrich-Heine-Universitaet Duesseldorf,
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

#include "SchedulerService.h"
#include "kernel/system/System.h"
#include "JobService.h"

namespace Kernel {

void SchedulerService::kickoffThread() {
    scheduler.getCurrentProcess().getThreadScheduler().getCurrentThread().run();
    scheduler.getCurrentProcess().getThreadScheduler().exit();
}

void SchedulerService::startScheduler() {
    scheduler.start();
}

void SchedulerService::ready(Process &process) {
    scheduler.ready(process);
}

void SchedulerService::ready(Thread &thread) {
    scheduler.getCurrentProcess().ready(thread);
}

Process *SchedulerService::createProcess(VirtualAddressSpace &addressSpace) {
    return new Process(scheduler, addressSpace);
}

void SchedulerService::releaseSchedulerLock() {
    scheduler.lock.release();
}

void SchedulerService::setSchedulerInitialized() {
    if (scheduler.isInitialized()) {
        Util::Exception::throwException(Util::Exception::ILLEGAL_STATE, "ThreadUtil is already initialized!");
    }

    SystemCall::registerSystemCall(Util::System::SystemCall::SCHEDULER_YIELD, [](uint32_t, va_list) -> Util::System::SystemCall::Result {
        System::getService<SchedulerService>().yield();
        return Util::System::SystemCall::Result::OK;
    });

    scheduler.setInitialized();
}

void SchedulerService::yield() {
    if (scheduler.isInitialized()) {
        scheduler.yield();
    }
}

bool SchedulerService::isSchedulerInitialized() {
    return scheduler.isInitialized();
}

}