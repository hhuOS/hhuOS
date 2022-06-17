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

#include "SchedulerCleaner.h"
#include "kernel/system/System.h"
#include "kernel/service/TimeService.h"
#include "lib/util/async/Thread.h"

namespace Kernel {

SchedulerCleaner::SchedulerCleaner() : processBitmap(ARRAY_SIZE), threadBitmap(ARRAY_SIZE) {
    processList = new Process *[ARRAY_SIZE];
    threadList = new Thread *[ARRAY_SIZE];

    for (uint32_t i = 0; i < ARRAY_SIZE; i++) {
        processList[i] = nullptr;
        threadList[i] = nullptr;
    }
}

SchedulerCleaner::~SchedulerCleaner() {
    SchedulerCleaner::run();
    delete processList;
    delete threadList;
}

void SchedulerCleaner::cleanup(Process *process) {
    auto index = processBitmap.findAndSet();
    if (index == Util::Memory::AtomicBitmap::INVALID_INDEX) {
        Util::Exception::throwException(Util::Exception::OUT_OF_BOUNDS, "Too many processes to cleanup!");
    }

    processList[index] = process;
}

void SchedulerCleaner::cleanup(Thread *thread) {
    auto index = threadBitmap.findAndSet();
    if (index == Util::Memory::AtomicBitmap::INVALID_INDEX) {
        Util::Exception::throwException(Util::Exception::OUT_OF_BOUNDS, "Too many threads to cleanup!");
    }

    threadList[index] = thread;
}

void SchedulerCleaner::run() {
    while (true) {
        cleanupProcesses();
        cleanupThreads();
        Util::Async::Thread::sleep({1, 0});
    }
}

void SchedulerCleaner::cleanupProcesses() {
    for (uint32_t i = 0; i < ARRAY_SIZE; i++) {
        if (processBitmap.check(i, true) && processList[i] != nullptr) {
            delete processList[i];
            processList[i] = nullptr;
            processBitmap.unset(i);
        }
    }
}

void SchedulerCleaner::cleanupThreads() {
    for (uint32_t i = 0; i < ARRAY_SIZE; i++) {
        if (threadBitmap.check(i, true) && threadList[i] != nullptr) {
            delete threadList[i];
            threadList[i] = nullptr;
            threadBitmap.unset(i);
        }
    }
}

}