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

#include "SchedulerService.h"
#include "kernel/system/System.h"
#include "JobService.h"
#include "FilesystemService.h"
#include "kernel/process/AddressSpaceCleaner.h"

namespace Kernel {

void SchedulerService::kickoffThread() {
    scheduler.getCurrentProcess().getThreadScheduler().getCurrentThread().run();
    scheduler.getCurrentProcess().getThreadScheduler().exit();
}

void SchedulerService::startScheduler() {
    System::getService<JobService>().registerJob(&cleaner, Job::LOW, Util::Time::Timestamp(1, 0));
    scheduler.start();
}

void SchedulerService::ready(Process &process) {
    scheduler.ready(process);
}

void SchedulerService::ready(Thread &thread) {
    scheduler.getCurrentProcess().ready(thread);
}

Process& SchedulerService::createProcess(VirtualAddressSpace &addressSpace, const Util::File::File &workingDirectory, const Util::File::File &standardOut) {
    auto *process = new Process(scheduler, addressSpace, workingDirectory);
    // Create standard out file descriptor
    if (System::isServiceRegistered(FilesystemService::SERVICE_ID)) {
        process->getFileDescriptorManager().openFile(standardOut.getCanonicalPath());
    }

    return *process;
}

void SchedulerService::releaseSchedulerLock() {
    scheduler.lock.release();
}

void SchedulerService::setSchedulerInitialized() {
    if (scheduler.isInitialized()) {
        Util::Exception::throwException(Util::Exception::ILLEGAL_STATE, "ThreadUtil is already initialized!");
    }

    SystemCall::registerSystemCall(Util::System::SCHEDULER_YIELD, [](uint32_t, va_list) -> Util::System::Result {
        System::getService<SchedulerService>().yield();
        return Util::System::Result::OK;
    });

    SystemCall::registerSystemCall(Util::System::SCHEDULER_EXIT, [](uint32_t paramCount, va_list arguments) -> Util::System::Result {
        int32_t exitCode = 0;
        if (paramCount >= 1) {
            exitCode = va_arg(arguments, int32_t);
        }

        auto &schedulerService = System::getService<SchedulerService>();
        schedulerService.exitCurrentProcess(exitCode);

        return Util::System::Result::OK;
    });

    scheduler.setInitialized();
}

void SchedulerService::yield() {
    if (scheduler.isInitialized()) {
        scheduler.yield();
    }
}

bool SchedulerService::isSchedulerInitialized() const {
    return scheduler.isInitialized();
}

Process& SchedulerService::getCurrentProcess() {
    return scheduler.getCurrentProcess();
}

Thread& SchedulerService::getCurrentThread() {
    return scheduler.getCurrentProcess().getThreadScheduler().getCurrentThread();
}

void SchedulerService::cleanup(Process *process) {
    cleaner.cleanup(process);
}

void SchedulerService::cleanup(Thread *thread) {
    cleaner.cleanup(thread);
}

void SchedulerService::exitCurrentProcess(int32_t exitCode) {
    getCurrentProcess().getThreadScheduler().killAllThreadsButCurrent();
    auto &cleanerThread = Thread::createKernelThread("Cleaner", new AddressSpaceCleaner());
    ready(cleanerThread);
    getCurrentProcess().setExitCode(exitCode);
    getCurrentProcess().getThreadScheduler().exit();
}

}