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

#include "ProcessScheduler.h"
#include "asm_interface.h"

namespace Kernel {

ProcessScheduler::~ProcessScheduler() {
    while (!processQueue.isEmpty()) {
        delete processQueue.pop();
    }
}

void ProcessScheduler::setInitialized() {
    initialized = 0x123456;
}

uint32_t ProcessScheduler::isInitialized() const {
    return initialized;
}

void ProcessScheduler::start() {
    lock.acquire();

    if (!isProcessWaiting()) {
        Util::Exception::throwException(Util::Exception::ILLEGAL_STATE, "ThreadUtil: No process is waiting to be scheduled!");
    }

    currentProcess = &getNextProcess();

    start_first_thread(currentProcess->getThreadScheduler().getNextThread(false).getContext());
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
    if (!initialized) {
        Util::Exception::throwException(Util::Exception::ILLEGAL_STATE, "ThreadUtil: 'exitProcess' called but threadScheduler is not initialized!");
    }

    lock.acquire();
    if (!isProcessWaiting()) {
        Util::Exception::throwException(Util::Exception::ILLEGAL_STATE, "ThreadUtil: No process is waiting to be scheduled!");
    }

    processQueue.remove(currentProcess);

    // TODO: Delete process (currently not possible, because the instance is still needed for dispatching)
    dispatch(getNextProcess(), false);
}

void ProcessScheduler::kill(Process &process) {
    if (!initialized) {
        Util::Exception::throwException(Util::Exception::ILLEGAL_STATE,"ThreadUtil: 'kill' called but threadScheduler is not initialized!");
    }

    lock.acquire();
    if (process.getId() == currentProcess->getId()) {
        Util::Exception::throwException(Util::Exception::ILLEGAL_STATE,"ThreadUtil: A process is trying to kill itself... Use 'exitProcess' instead!");
    }

    processQueue.remove(&process);
    lock.release();

    delete &process;
}

bool ProcessScheduler::isProcessWaiting() {
    return !processQueue.isEmpty();
}

Process &ProcessScheduler::getCurrentProcess() {
    return *currentProcess;
}

uint32_t ProcessScheduler::getProcessCount() {
    return processQueue.size();
}

uint32_t ProcessScheduler::getThreadCount() {
    return processQueue.size();
}

Process &ProcessScheduler::getNextProcess() {
    if (!isProcessWaiting()) {
        Util::Exception::throwException(Util::Exception::ILLEGAL_STATE, "ThreadUtil: No thread is waiting to be scheduled!");
    }

    Process *process = processQueue.pop();
    processQueue.push(process);

    return *process;
}

void ProcessScheduler::yield() {
    if (!isProcessWaiting()) {
        Util::Exception::throwException(Util::Exception::ILLEGAL_STATE, "No process is running!");
    }

    if(lock.tryAcquire()) {
        dispatch(getNextProcess(), true);
    } else {
        return;
    }
}

void ProcessScheduler::forceYield() {
    if (!isProcessWaiting()) {
        Util::Exception::throwException(Util::Exception::ILLEGAL_STATE, "No process is running!");
    }

    lock.acquire();
    dispatch(getNextProcess(), false);
}

void ProcessScheduler::yieldFromThreadScheduler(bool tryLock) {
    if (!isProcessWaiting()) {
        lock.release();
        return;
    }

    dispatch(getNextProcess(), tryLock);
}

void ProcessScheduler::dispatch(Process &next, bool tryLock) {
    if (!initialized) {
        Util::Exception::throwException(Util::Exception::ILLEGAL_STATE,"ThreadUtil: 'dispatch' called but threadScheduler is not initialized!");
    }

    Thread &oldThread = currentProcess->getThreadScheduler().getCurrentThread();
    next.getThreadScheduler().yield(oldThread, next, tryLock);
}

}