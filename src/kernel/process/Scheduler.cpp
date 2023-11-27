/*
 * Copyright (C) 2018-2023 Heinrich-Heine-Universitaet Duesseldorf,
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
#include "lib/util/base/Exception.h"
#include "lib/util/time/Timestamp.h"
#include "lib/util/collection/Iterator.h"

namespace Kernel {

bool Scheduler::fpuAvailable = Device::Fpu::isAvailable();

Scheduler::~Scheduler() {
    while (!readyQueue.isEmpty()) {
        delete readyQueue.poll();
    }
}

Thread& Scheduler::getCurrentThread() {
    if (!scheduler_initialized) {
        Util::Exception::throwException(Util::Exception::ILLEGAL_STATE, "Scheduler: Trying to get current thread before initialization!");
    }

    return *currentThread;
}

void Scheduler::start() {
    readyQueueLock.acquire();
    if (readyQueue.isEmpty()) {
        readyQueueLock.release();
        Util::Exception::throwException(Util::Exception::ILLEGAL_STATE, "Scheduler: No thread registered!");
    }

    auto *thread = readyQueue.poll();
    currentThread = thread;

    start_first_thread(currentThread->getContext());
}

void Scheduler::ready(Thread &thread) {
    readyQueueLock.acquire();
    if (readyQueue.contains(&thread)) {
        Util::Exception::throwException(Util::Exception::INVALID_ARGUMENT, "Scheduler: Thread is already running!");
    }

    readyQueue.offer(&thread);
    thread.getParent().addThread(thread);
    readyQueueLock.release();
}

void Scheduler::exit() {
    readyQueueLock.acquire();

    readyQueue.remove(currentThread);
    currentThread->getParent().removeThread(*currentThread);
    currentThread->unblockJoinList();
    System::getService<SchedulerService>().cleanup(currentThread);

    readyQueueLock.release();
    block();
}

void Scheduler::kill(Thread &thread) {
    if (thread.getId() == currentThread->getId()) {
        Util::Exception::throwException(Util::Exception::INVALID_ARGUMENT,"Scheduler: A thread cannot kill itself!");
    }

    readyQueueLock.acquire();
    sleepQueueLock.acquire();
    sleepList.remove(SleepEntry{&thread, 0});
    sleepQueueLock.release();

    readyQueue.remove(&thread);
    thread.getParent().removeThread(thread);
    thread.unblockJoinList();
    readyQueueLock.release();

    System::getService<SchedulerService>().cleanup(&thread);
}

void Scheduler::killWithoutLock(Thread &thread) {
    if (thread.getId() == currentThread->getId()) {
        Util::Exception::throwException(Util::Exception::INVALID_ARGUMENT,"Scheduler: A thread cannot kill itself!");
    }

    sleepQueueLock.acquire();
    sleepList.remove(SleepEntry{&thread, 0});
    sleepQueueLock.release();

    readyQueue.remove(&thread);
    thread.getParent().removeThread(thread);
    thread.unblockJoinList();

    System::getService<SchedulerService>().cleanup(&thread);
}

void Scheduler::yield() {
    if (!scheduler_initialized) {
        return;
    }

    if (!readyQueueLock.tryAcquire()) {
        return;
    }

    checkSleepList();

    auto *current = currentThread;
    auto *next = readyQueue.poll();
    currentThread = next;

    readyQueue.offer(current);

    if (fpuAvailable) {
        Device::Fpu::armFpuMonitor();
    }
    System::getService<Kernel::MemoryService>().switchAddressSpace(next->getParent().getAddressSpace());
    switch_context(&current->kernelContext, &next->kernelContext);
}

uint32_t Scheduler::getThreadCount() const {
    return readyQueue.size();
}

void Scheduler::block() {
    readyQueueLock.acquire();

    do {
        checkSleepList();
    } while (readyQueue.isEmpty());

    auto *current = currentThread;
    auto *next = readyQueue.poll();
    currentThread = next;

    // Thread has enqueued itself into sleep list and waited so long, that it dequeued itself in the meantime
    if (current == next) {
        return;
    }

    if (fpuAvailable) {
        Device::Fpu::armFpuMonitor();
    }
    System::getService<Kernel::MemoryService>().switchAddressSpace(next->getParent().getAddressSpace());
    switch_context(&current->kernelContext, &next->kernelContext);
}

void Scheduler::unblock(Thread &thread) {
    readyQueue.offer(&thread);
}

void Scheduler::sleep(const Util::Time::Timestamp &time) {
    auto systemTime = System::getService<TimeService>().getSystemTime().toMilliseconds();

    sleepQueueLock.acquire();
    sleepList.add(SleepEntry{currentThread, systemTime + time.toMilliseconds()});
    sleepQueueLock.release();

    block();
}

void Scheduler::checkSleepList() {
    if (sleepQueueLock.tryAcquire()) {
        auto systemTime = System::getService<TimeService>().getSystemTime().toMilliseconds();
        for (uint32_t i = 0; i < sleepList.size(); i++) {
            const auto &entry = sleepList.get(i);
            if (systemTime >= entry.wakeupTime) {
                readyQueue.offer(entry.thread);
                sleepList.remove(entry);
            }
        }
        sleepQueueLock.release();
    }
}

Thread* Scheduler::getThread(uint32_t id) {
    readyQueueLock.acquire();
    sleepQueueLock.acquire();

    for (auto *thread : readyQueue) {
        if (thread->getId() == id) {
            sleepQueueLock.release();
            readyQueueLock.release();
            return thread;
        }
    }

    for (auto &sleepEntry : sleepList) {
        if (sleepEntry.thread->getId() == id) {
            sleepQueueLock.release();
            readyQueueLock.release();
            return sleepEntry.thread;
        }
    }

    sleepQueueLock.release();
    readyQueueLock.release();
    return nullptr;
}

bool Scheduler::SleepEntry::operator!=(const Scheduler::SleepEntry &other) const {
    return thread->getId() != other.thread->getId();
}

}
