/*
 * Copyright (C) 2018 Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schoettner
 * Heinrich-Heine University
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

#include <devices/cpu/Cpu.h>
#include <lib/libc/printf.h>
#include <devices/misc/Pic.h>
#include <kernel/Kernel.h>
#include <kernel/services/PortService.h>
#include <devices/timer/Pit.h>
#include <kernel/services/SoundService.h>
#include <lib/system/SystemCall.h>
#include <kernel/threads/priority/AccessArrayThreadPriority.h>
#include "kernel/threads/Scheduler.h"
#include "Scheduler.h"
#include "IdleThread.h"

extern "C" {
    void startThread(Context* first);
    void switchContext(Context **current, Context **next);
    void setSchedInit();
    void releaseSchedulerLock();
    void allowPitInterrupts();
}

void releaseSchedulerLock() {
    Scheduler::getInstance().lock.release();
}

void allowPitInterrupts() {
    Pic::getInstance().allow(Pic::Interrupt::PIT);
}

Scheduler::Scheduler(ThreadPriority &priority) : priority(priority), readyQueues(priority.getPriorityCount()) {

    SystemCall::registerSystemCall(SystemCall::SCHEDULER_YIELD, [](){Scheduler::getInstance().yield();});
    SystemCall::registerSystemCall(SystemCall::SCHEDULER_BLOCK, [](){Scheduler::getInstance().block();});
}

Scheduler& Scheduler::getInstance() noexcept {

    static AccessArrayThreadPriority priority(5);

    static Scheduler instance(priority);

    return instance;
}

void Scheduler::startUp() {

    lock.acquire();

    if (!isThreadWaiting()) {

        Cpu::throwException(Cpu::Exception::ILLEGAL_STATE);
    }

    currentThread = getNextThread();

    initialized = true;

    setSchedInit();

    Pit::getInstance().setYieldable(this);

    startThread(currentThread->context);
}

void Scheduler::ready(Thread& that) {

    lock.acquire();

    readyQueues[that.getPriority()].push(&that);

    lock.release();
}

void Scheduler::exit() {

    lock.acquire();

    if (!initialized) {

        Cpu::throwException(Cpu::Exception::ILLEGAL_STATE);
    }
    
    if (!isThreadWaiting()) {

        Cpu::throwException(Cpu::Exception::ILLEGAL_STATE);
    }

    Thread* next = getNextThread();
    
    dispatch (*next);
}

void Scheduler::kill(Thread& that) {

    lock.acquire();

    if (!initialized) {

        Cpu::throwException(Cpu::Exception::ILLEGAL_STATE);
    }

    if(that.getId() == currentThread->getId()) {

        Cpu::throwException(Cpu::Exception::ILLEGAL_STATE);
    }

    readyQueues[that.getPriority()].remove(&that);

    lock.release();
}

void Scheduler::yield() {

    if (!initialized) {

        Cpu::throwException(Cpu::Exception::ILLEGAL_STATE);
    }

    if (!isThreadWaiting()) {

        return;
    }

    if(!Cpu::isInterrupted()) {
        Cpu::softInterrupt(SystemCall::SCHEDULER_YIELD);

        return;
    }

    if(lock.tryLock()) {

        Thread *next = getNextThread();

        readyQueues[currentThread->getPriority()].push(currentThread);

        dispatch(*next);
    }
}

void Scheduler::block() {

    if (!initialized) {

        Cpu::throwException(Cpu::Exception::ILLEGAL_STATE);
    }
    
    if (!isThreadWaiting()) {

        Cpu::throwException(Cpu::Exception::ILLEGAL_STATE);
    }

    if(!Cpu::isInterrupted()) {
        Cpu::softInterrupt(SystemCall::SCHEDULER_BLOCK);

        return;
    }

    lock.acquire();

    Thread* next = getNextThread();
    
    dispatch (*next);
}

void Scheduler::deblock(Thread &that) {

    lock.acquire();

    if (!initialized) {

        Cpu::throwException(Cpu::Exception::ILLEGAL_STATE);
    }

    readyQueues[that.getPriority()].push(&that);

    lock.release();
}

void Scheduler::dispatch(Thread &next) {

    if (!initialized) {

        Cpu::throwException(Cpu::Exception::ILLEGAL_STATE);
    }

    Thread* current = currentThread;

    currentThread = &next;

    switchContext(&current->context, &next.context);
}

Thread* Scheduler::getNextThread() {

    if(!isThreadWaiting()) {
        Cpu::throwException(Cpu::Exception::ILLEGAL_STATE);
    }

    Util::BlockingQueue<Thread*> *currentQueue = &readyQueues[priority.getNextPriority()];

    while(currentQueue->isEmpty()) {
        currentQueue = &readyQueues[priority.getNextPriority()];
    }

    Thread *ret = currentQueue->pop();

    return ret;
}

bool Scheduler::isInitialized() {

    return initialized;
}

bool Scheduler::isThreadWaiting() {

    for(const auto &queue : readyQueues) {
        if(!queue.isEmpty()) {
            return true;
        }
    }

    return false;
}

uint32_t Scheduler::getThreadCount() {

    uint32_t count = 0;

    for(const auto &queue : readyQueues) {
        count += queue.size();
    }

    return count;
}

uint8_t Scheduler::getMaxPriority() {

    return static_cast<uint8_t>(readyQueues.length() - 1);
}

uint8_t Scheduler::changePriority(Thread &thread, uint8_t priority) {

    priority = static_cast<uint8_t>((thread.getPriority() > getMaxPriority()) ? (getMaxPriority()) : thread.getPriority());

    lock.acquire();

    if(&thread == currentThread) {
        lock.release();

        return priority;
    }

    readyQueues[thread.getPriority()].remove(&thread);

    readyQueues[priority].push(&thread);

    lock.release();

    return priority;
}


