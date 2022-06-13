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

#include "ThreadScheduler.h"
#include "ProcessScheduler.h"
#include "kernel/system/System.h"
#include "asm_interface.h"
#include "device/cpu/Fpu.h"

namespace Kernel {

bool ThreadScheduler::fpuAvailable = Device::Fpu::isAvailable();

ThreadScheduler::ThreadScheduler(ProcessScheduler &parent) : parent(parent) {}

ThreadScheduler::~ThreadScheduler() {
    while (!threadQueue.isEmpty()) {
        delete threadQueue.pop();
    }
}

void ThreadScheduler::ready(Thread &thread) {
    if (currentThread == nullptr) {
        currentThread = &thread;
    }

    if (threadQueue.contains(&thread)) {
        Util::Exception::throwException(Util::Exception::INVALID_ARGUMENT, "Scheduler: Thread is already running!");
    }

    lock.acquire();
    threadQueue.push(&thread);
    lock.release();
}

void ThreadScheduler::exit() {
    lock.acquire();
    threadQueue.remove(currentThread);
    lock.release();

    currentThread->unblockJoinList();

    System::getService<SchedulerService>().cleanup(currentThread);
    parent.forceYield();
}

void ThreadScheduler::kill(Thread &thread) {
    if (thread.getId() == currentThread->getId()) {
        Util::Exception::throwException(Util::Exception::INVALID_ARGUMENT,"Scheduler: A thread is trying to kill itself... Use 'exit()' instead!");
    }

    lock.acquire();
    threadQueue.remove(&thread);
    thread.unblockJoinList();
    lock.release();

    System::getService<SchedulerService>().cleanup(currentThread);
}

Thread& ThreadScheduler::getCurrentThread() {
    return *currentThread;
}

Thread& ThreadScheduler::getNextThread() {
    Thread *thread = threadQueue.pop();
    threadQueue.push(thread);

    return *thread;
}

void ThreadScheduler::yield(Thread &oldThread, Process &nextProcess, bool force) {
    if (force) {
        lock.acquire();
    } else {
        if (!lock.tryAcquire()) {
            parent.lock.release();
            return;
        }
    }

    Thread &nextThread = getNextThread();
    lock.release();

    System::getService<Kernel::MemoryService>().switchAddressSpace(nextProcess.getAddressSpace());
    parent.currentProcess = &nextProcess;

    dispatch(oldThread, nextThread);
}

void ThreadScheduler::dispatch(Thread &current, Thread &next) {
    currentThread = &next;
    if (fpuAvailable) {
        Device::Fpu::armFpuMonitor();
    }
    switch_context(&current.kernelContext, &next.kernelContext);
}

void ThreadScheduler::killAllThreadsButCurrent() {
    for (const auto thread : threadQueue) {
        if (thread->getId() != currentThread->getId()) {
            kill(*thread);
        }
    }
}

uint32_t ThreadScheduler::getThreadCount() const {
    return threadQueue.size();
}

void ThreadScheduler::block() {
    lock.acquire();
    threadQueue.remove(currentThread);
    lock.release();
}

void ThreadScheduler::unblock(Thread &thread) {
    lock.acquire();
    threadQueue.push(&thread);
    lock.release();
}

}
