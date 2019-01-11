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
#include "kernel/threads/Scheduler.h"
#include "Scheduler.h"
#include "IdleThread.h"

extern "C" {
    void startThread(Context* first);
    void switchContext(Context **current, Context **next);
    void setSchedInit();
    void releaseSchedulerLock();
}

void releaseSchedulerLock() {
    Scheduler::getInstance().lock.release();
}

Scheduler::Scheduler(uint8_t priorityCount) : readyQueues(priorityCount < 2 ? 2 : priorityCount),
        accessArray(((readyQueues.length()) * (readyQueues.length() - 1)) / 2u) {

    SystemCall::registerSystemCall(SystemCall::SCHEDULER_YIELD, [](){Scheduler::getInstance().yield();});
    SystemCall::registerSystemCall(SystemCall::SCHEDULER_BLOCK, [](){Scheduler::getInstance().block();});

    uint8_t tmp[priorityCount];

    for(uint8_t i = 0; i < priorityCount; i++) {
        tmp[i] = i;
    }

    uint8_t index = getMaxPriority();

    for(uint32_t i = 0; i < accessArray.length(); i++) {
        while(tmp[index] == 0) {
            index++;
            index %= priorityCount;
        }

        accessArray[i] = index;
        tmp[index]--;

        index++;
        index %= priorityCount;
    }
}

Scheduler& Scheduler::getInstance()  {

    static Scheduler instance;

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

    uint8_t counter = 0;

    while(readyQueues[accessArray[accessCounter % accessArray.length()]].isEmpty()) {
        accessCounter++;
        counter++;

        if(counter > accessArray.length()) {
            return readyQueues[0].pop();
        }
    }

    Thread *ret = readyQueues[accessArray[accessCounter++ % accessArray.length()]].pop();

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


