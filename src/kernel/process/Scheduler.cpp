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

#include "Scheduler.h"

#include "kernel/system/System.h"
#include "kernel/service/TimeService.h"
#include "asm_interface.h"
#include "device/cpu/Fpu.h"
#include "kernel/process/Process.h"
#include "kernel/process/Thread.h"
#include "kernel/service/MemoryService.h"
#include "kernel/service/SchedulerService.h"
#include "lib/util/Exception.h"
#include "lib/util/time/Timestamp.h"

extern uint32_t scheduler_initialized;

namespace Kernel {

bool Scheduler::fpuAvailable = Device::Fpu::isAvailable();

Scheduler::~Scheduler() {
    while (!threadQueue.isEmpty()) {
        delete threadQueue.poll();
    }
}

void Scheduler::start() {
    lock.acquire();
    currentThread = &getNextThread();
    start_first_thread(currentThread->getContext());
}

void Scheduler::ready(Thread &thread) {
    if (currentThread == nullptr) {
        currentThread = &thread;
    }

    if (threadQueue.contains(&thread)) {
        Util::Exception::throwException(Util::Exception::INVALID_ARGUMENT, "Scheduler: Thread is already running!");
    }

    lock.acquire();
    threadQueue.offer(&thread);
    thread.getParent().addThread(thread);
    lock.release();
}

void Scheduler::exit() {
    lock.acquire();
    threadQueue.remove(currentThread);
    currentThread->getParent().removeThread(*currentThread);
    lock.release();

    currentThread->unblockJoinList();

    System::getService<SchedulerService>().cleanup(currentThread);
    yield(true);
}

void Scheduler::kill(Thread &thread) {
    if (thread.getId() == currentThread->getId()) {
        Util::Exception::throwException(Util::Exception::INVALID_ARGUMENT,"Scheduler: A thread cannot kill itself!");
    }

    sleepLock.acquire();
    sleepList.remove(SleepEntry{&thread, 0});
    sleepLock.release();

    lock.acquire();
    threadQueue.remove(&thread);
    thread.getParent().removeThread(thread);
    thread.unblockJoinList();
    lock.release();

    System::getService<SchedulerService>().cleanup(&thread);
}

void Scheduler::killWithoutLock(Thread &thread) {
    if (thread.getId() == currentThread->getId()) {
        Util::Exception::throwException(Util::Exception::INVALID_ARGUMENT,"Scheduler: A thread cannot kill itself!");
    }

    sleepLock.acquire();
    sleepList.remove(SleepEntry{&thread, 0});
    sleepLock.release();

    threadQueue.remove(&thread);
    thread.getParent().removeThread(thread);
    thread.unblockJoinList();

    System::getService<SchedulerService>().cleanup(&thread);
}

Thread& Scheduler::getCurrentThread() {
    return *currentThread;
}

Thread& Scheduler::getNextThread() {
    Thread *thread = threadQueue.poll();
    threadQueue.offer(thread);

    return *thread;
}

void Scheduler::yield(bool force) {
    if (!scheduler_initialized) {
        return;
    }

    if (force) {
        lock.acquire();
    } else if (!lock.tryAcquire()) {
        return;
    }

    do {
        checkSleepList();
    } while (threadQueue.isEmpty());

    Thread &nextThread = getNextThread();

    System::getService<Kernel::MemoryService>().switchAddressSpace(nextThread.getParent().getAddressSpace());
    dispatch(nextThread);
}

void Scheduler::dispatch(Thread &nextThread) {
    auto &oldThread = *currentThread;
    currentThread = &nextThread;
    if (fpuAvailable) {
        Device::Fpu::armFpuMonitor();
    }

    switch_context(&oldThread.kernelContext, &nextThread.kernelContext);
}

uint32_t Scheduler::getThreadCount() const {
    return threadQueue.size();
}

void Scheduler::block() {
    lock.acquire();
    threadQueue.remove(currentThread);
    lock.release();

    yield(true);
}

void Scheduler::unblock(Thread &thread) {
    lock.acquire();
    threadQueue.offer(&thread);
    lock.release();
}

void Scheduler::sleep(const Util::Time::Timestamp &time) {
    auto systemTime = System::getService<TimeService>().getSystemTime().toMilliseconds();

    sleepLock.acquire();
    sleepList.add(SleepEntry{currentThread, systemTime + time.toMilliseconds()});
    sleepLock.release();

    block();
}

void Scheduler::checkSleepList() {
    if (sleepLock.tryAcquire()) {
        auto systemTime = System::getService<TimeService>().getSystemTime().toMilliseconds();
        for (uint32_t i = 0; i < sleepList.size(); i++) {
            const auto &entry = sleepList.get(i);
            if (systemTime >= entry.wakeupTime) {
                threadQueue.offer(entry.thread);
                sleepList.remove(entry);
            }
        }
        sleepLock.release();
    }
}

Thread* Scheduler::getThread(uint32_t id) {
    lock.acquire();
    sleepLock.acquire();
    for (auto *thread : threadQueue) {
        if (thread->getId() == id) {
            sleepLock.release();
            lock.release();
            return thread;
        }
    }

    for (auto &sleepEntry : sleepList) {
        if (sleepEntry.thread->getId() == id) {
            sleepLock.release();
            lock.release();
            return sleepEntry.thread;
        }
    }

    sleepLock.release();
    lock.release();
    return nullptr;
}

bool Scheduler::SleepEntry::operator!=(const Scheduler::SleepEntry &other) const {
    return thread->getId() != other.thread->getId();
}

}
