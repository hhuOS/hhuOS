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

#include "kernel/system/System.h"
#include "asm_interface.h"
#include "ProcessScheduler.h"

extern uint32_t scheduler_initialized;

namespace Kernel {

ProcessScheduler::~ProcessScheduler() {
    lock.acquire();
    while (!processQueue.isEmpty()) {
        delete processQueue.pop();
    }
    lock.release();
}

void ProcessScheduler::start() {
    lock.acquire();
    currentProcess = &getNextProcess();
    start_first_thread(currentProcess->getThreadScheduler().getNextThread().getContext());
}

void ProcessScheduler::ready(Process &process) {
    if (currentProcess == nullptr) {
        currentProcess = &process;
    }

    lock.acquire();
    processQueue.push(&process);
    lock.release();
}

void ProcessScheduler::exit() {
    lock.acquire();
    processQueue.remove(currentProcess);

    System::getService<SchedulerService>().cleanup(currentProcess);
    dispatch(getNextProcess(), false);
}

void ProcessScheduler::kill(Process &process) {
    if (process.getId() == currentProcess->getId()) {
        exit();
        return;
    }

    lock.acquire();
    processQueue.remove(&process);
    lock.release();

    delete &process;
}

Process &ProcessScheduler::getCurrentProcess() {
    return *currentProcess;
}

uint32_t ProcessScheduler::getProcessCount() {
    return processQueue.size();
}

Process &ProcessScheduler::getNextProcess() {
    if (processQueue.isEmpty()) {
        Util::Exception::throwException(Util::Exception::ILLEGAL_STATE, "Scheduler: No process is running!");
    }

    Process *process;
    uint32_t threadCount;

    do {
        process = processQueue.pop();
        processQueue.push(process);
        threadCount = process->getThreadCount();
    } while (threadCount == 0);

    return *process;
}

void ProcessScheduler::yield() {
    if (scheduler_initialized && lock.tryAcquire()) {
        dispatch(getNextProcess(), false);
    }
}

void ProcessScheduler::forceYield() {
    lock.acquire();
    dispatch(getNextProcess(), true);
}

void ProcessScheduler::dispatch(Process &next, bool force) {
    auto &oldThread = currentProcess->getThreadScheduler().getCurrentThread();
    next.getThreadScheduler().yield(oldThread, next, force);
}

void ProcessScheduler::blockCurrentThread() {
    currentProcess->getThreadScheduler().block();
    forceYield();
}

Process* ProcessScheduler::getProcess(uint32_t id) {
    lock.acquire();
    for (auto *process : processQueue) {
        if (process->getId() == id) {
            return lock.releaseAndReturn(process);
        }
    }

    return lock.releaseAndReturn(nullptr);
}

}