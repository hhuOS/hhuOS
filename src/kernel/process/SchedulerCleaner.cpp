/*
 * Copyright (C) 2018-2024 Heinrich-Heine-Universitaet Duesseldorf,
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

#include "SchedulerCleaner.h"

#include "lib/util/async/Thread.h"
#include "kernel/process/Process.h"
#include "kernel/process/Thread.h"
#include "lib/util/base/Exception.h"
#include "lib/util/time/Timestamp.h"
#include "kernel/service/Service.h"
#include "kernel/service/ProcessService.h"
#include "kernel/process/Scheduler.h"

namespace Kernel {

SchedulerCleaner::SchedulerCleaner() : processQueue(16), threadQueue(16) {}

SchedulerCleaner::~SchedulerCleaner() {
    SchedulerCleaner::run();
}

void SchedulerCleaner::cleanup(Process *process) {
    if (!processQueue.offer(process)) {
        Util::Exception::throwException(Util::Exception::OUT_OF_BOUNDS, "Too many processes to cleanup!");
    }
}

void SchedulerCleaner::cleanup(Thread *thread) {
    if (!threadQueue.offer(thread)) {
        Util::Exception::throwException(Util::Exception::OUT_OF_BOUNDS, "Too many threads to cleanup!");
    }
}

void SchedulerCleaner::run() {
    while (true) {
        cleanupThreads();
        cleanupProcesses();
        Util::Async::Thread::sleep(Util::Time::Timestamp::ofSeconds(1));
    }
}

void SchedulerCleaner::cleanupProcesses() {
    while (processQueue.size() > 0) {
        delete processQueue.poll();
    }
}

void SchedulerCleaner::cleanupThreads() {
    auto &scheduler = Service::getService<ProcessService>().getScheduler();

    while (threadQueue.size() > 0) {
        auto *thread = threadQueue.poll();

        if (scheduler.getThread(thread->getId())) {
            // Thread is still inside ready queue -> Wait until the scheduler has finished blocking the thread
            threadQueue.add(thread);
            Util::Async::Thread::yield();
        } else {
            delete thread;
        }
    }
}

}