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

#include "Scheduler.h"

#include <lib/util/base/Address.h>

#include "kernel/service/TimeService.h"
#include "device/cpu/Fpu.h"
#include "kernel/process/Process.h"
#include "kernel/process/Thread.h"
#include "kernel/service/MemoryService.h"
#include "kernel/service/SchedulerService.h"
#include "lib/util/base/Exception.h"
#include "lib/util/time/Timestamp.h"
#include "kernel/log/Log.h"
#include "kernel/service/InterruptService.h"
#include "kernel/interrupt/InterruptVector.h"
#include "kernel/service/Service.h"
#include "lib/util/async/Atomic.h"
#include "lib/util/collection/Array.h"
#include "lib/util/collection/HashMap.h"
#include "lib/util/collection/Iterator.h"

namespace Kernel {

Scheduler::Scheduler() {
    defaultFpuContext = static_cast<uint8_t*>(Service::getService<MemoryService>().allocateKernelMemory(512, 16));
    Util::Address<uint32_t>(defaultFpuContext).setRange(0, 512);

    if (Device::Fpu::isAvailable()) {
        LOG_INFO("FPU detected -> Enabling FPU context switching");
        fpu = new Device::Fpu(defaultFpuContext);
    } else {
        LOG_WARN("No FPU present");
    }
}

Scheduler::~Scheduler() {
    while (!readyQueue.isEmpty()) {
        delete readyQueue.poll();
    }

    for (auto id : joinMap.keys()) {
        delete joinMap.remove(id);
    }
}

void Scheduler::setInit() {
    initialized = true;
}

bool Scheduler::isInitialized() const {
    return initialized;
}

Thread& Scheduler::getCurrentThread() {
    if (!initialized) {
        readyQueueLock.release();
        Util::Exception::throwException(Util::Exception::ILLEGAL_STATE, "Scheduler: Trying to get current thread before initialization!");
    }

    return *currentThread;
}

Thread* Scheduler::getLastFpuThread() {
    return lastFpuThread;
}

void Scheduler::start() {
    readyQueueLock.acquire();
    if (readyQueue.isEmpty()) {
        readyQueueLock.release();
        Util::Exception::throwException(Util::Exception::ILLEGAL_STATE, "Scheduler: No thread registered!");
    }

    auto *thread = readyQueue.poll();
    currentThread = thread;

    Thread::startFirstThread(*currentThread);
}

void Scheduler::ready(Thread &thread) {
    readyQueueLock.acquire();
    if (readyQueue.contains(&thread)) {
        Util::Exception::throwException(Util::Exception::INVALID_ARGUMENT, "Scheduler: Thread is already running!");
    }

    readyQueue.offer(&thread);
    thread.getParent().addThread(thread);

    joinLock.acquire();
    joinMap.put(thread.getId(), new Util::ArrayList<Thread*>());
    joinLock.release();

    readyQueueLock.release();
}

void Scheduler::exit() {
    readyQueueLock.acquire();

    readyQueue.remove(currentThread);
    currentThread->getParent().removeThread(*currentThread);

    unblockJoinList(*currentThread);

    resetLastFpuThread(*currentThread);
    Service::getService<SchedulerService>().cleanup(currentThread);

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
    unblockJoinList(thread);
    readyQueueLock.release();

    resetLastFpuThread(thread);
    Service::getService<SchedulerService>().cleanup(&thread);
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
    unblockJoinList(thread);

    resetLastFpuThread(thread);
    Service::getService<SchedulerService>().cleanup(&thread);
}

void Scheduler::yield(bool interrupt) {
    if (!initialized) {
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

    if (fpu != nullptr) {
        Device::Fpu::armFpuMonitor();
    }

    if (interrupt) {
        auto &interruptService = Service::getService<InterruptService>();
        auto vector = interruptService.usesApic() ? InterruptVector::APICTIMER : InterruptVector::PIT;
        interruptService.sendEndOfInterrupt(vector);
    }

    Thread::switchThread(*current, *next);
}

void Scheduler::switchFpuContext() {
    if (fpu == nullptr) {
        Util::Exception::throwException(Util::Exception::DEVICE_NOT_AVAILABLE, "FPU not found!");
    }

    readyQueueLock.acquire();

    // Disable FPU monitoring (will be enabled by scheduler at next thread switch)
    Device::Fpu::disarmFpuMonitor();

    if (currentThread == lastFpuThread) {
        readyQueueLock.release();
        return;
    }

    fpu->switchContext();

    lastFpuThread = currentThread;
    readyQueueLock.release();
}

uint32_t Scheduler::getThreadCount() const {
    return readyQueue.size();
}

uint8_t* Scheduler::getDefaultFpuContext() {
    return defaultFpuContext;
}

void Scheduler::unlockReadyQueue() {
    readyQueueLock.release();
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

    if (fpu != nullptr) {
        Device::Fpu::armFpuMonitor();
    }

    Thread::switchThread(*current, *next);
}

void Scheduler::unblock(Thread &thread) {
    readyQueue.offer(&thread);
}

void Scheduler::sleep(const Util::Time::Timestamp &time) {
    auto systemTime = Service::getService<TimeService>().getSystemTime().toMilliseconds();

    sleepQueueLock.acquire();
    sleepList.add(SleepEntry{currentThread, systemTime + time.toMilliseconds()});
    sleepQueueLock.release();

    block();
}

void Scheduler::join(const Thread& thread) {
    joinLock.acquire();

    auto *joinList = joinMap.get(thread.getId());
    joinList->add(currentThread);

    joinLock.release();
    block();
}

void Scheduler::checkSleepList() {
    if (sleepQueueLock.tryAcquire()) {
        auto systemTime = Service::getService<TimeService>().getSystemTime().toMilliseconds();
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

void Scheduler::unblockJoinList(Thread& thread) {
    // Ready threads that are joining on the current threads
    joinLock.acquire();
    auto *joinList = joinMap.get(thread.getId());
    for (auto *thread : *joinList) {
        readyQueue.offer(thread);
    }

    joinMap.remove(thread.getId());
    delete joinList;
    joinLock.release();
}

void Scheduler::resetLastFpuThread(Thread &terminatedThread) {
    Util::Async::Atomic<uint32_t> wrapper(reinterpret_cast<uint32_t&>(lastFpuThread));
    wrapper.compareAndSet(reinterpret_cast<uint32_t>(&terminatedThread), 0);
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
