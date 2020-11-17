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

#include <kernel/core/Management.h>
#include "device/cpu/Cpu.h"
#include "lib/libc/printf.h"
#include "device/misc/Pic.h"
#include "kernel/core/SystemCall.h"
#include "lib/system/priority/AccessArrayPriorityPattern.h"
#include "kernel/thread/Scheduler.h"
#include "IdleThread.h"
#include "lib/libc/sprintf.h"

namespace Kernel {

extern "C" {
    void startThread(Context *first);
    void switchContext(Context **current, Context **next);
    void setSchedInit();
    void releaseSchedulerLock();
}

void releaseSchedulerLock() {
    Scheduler::getInstance().lock.release();
}

void allowPitInterrupts() {
    Pic::getInstance().allow(Pic::Interrupt::PIT);
}

Scheduler::Scheduler(PriorityPattern &priority) : currentThread(nullptr), priority(priority), readyQueues(priority.getPriorityCount()) {

    SystemCall::registerSystemCall(Standard::System::Call::SCHEDULER_YIELD, [](uint32_t paramCount, va_list params, Standard::System::Result *result) {
        if (Scheduler::getInstance().isInitialized()) {
            Scheduler::getInstance().yield();
            result->setStatus(Standard::System::Result::OK);
        } else {
            result->setStatus(Standard::System::Result::NOT_INITIALIZED);
        }
    });
}

Scheduler &Scheduler::getInstance() noexcept {

    static AccessArrayPriorityPattern priority(5);

    static Scheduler instance(priority);

    return instance;
}

void Scheduler::startUp() {

    lock.acquire();

    if (!isThreadWaiting()) {

        Cpu::throwException(Cpu::Exception::ILLEGAL_STATE, "Scheduler: No thread is waiting to be scheduled!");
    }

    currentThread = getNextThread();

    initialized = true;

    Kernel::Management::getTaskStateSegment().ss0 = 0x10;

    setSchedInit();

    startThread(currentThread->kernelContext);
}

void Scheduler::ready(Thread &that) {

    if (that.hasStarted()) {
        Cpu::throwException(Cpu::Exception::ILLEGAL_STATE, "Scheduler: Trying to start an already running thread!");
    }

    lock.acquire();

    readyQueues[that.getPriority()].push(&that);

    that.started = true;

    lock.release();
}

void Scheduler::exit() {

    lock.acquire();

    if (!initialized) {

        Cpu::throwException(Cpu::Exception::ILLEGAL_STATE, "Scheduler: 'exit' called but scheduler is not initialized!");
    }

    if (!isThreadWaiting()) {

        Cpu::throwException(Cpu::Exception::ILLEGAL_STATE, "Scheduler: No thread is waiting to be scheduled!");
    }

    Thread *next = getNextThread();

    currentThread->finished = true;

    dispatch(*next);
}

void Scheduler::kill(Thread &that) {

    lock.acquire();

    if (!initialized) {

        Cpu::throwException(Cpu::Exception::ILLEGAL_STATE,
                            "Scheduler: 'kill' called but scheduler is not initialized!");
    }

    if (that.getId() == currentThread->getId()) {

        Cpu::throwException(Cpu::Exception::ILLEGAL_STATE,
                            "Scheduler: A thread is trying to kill itself... Use 'exit' instead!");
    }

    readyQueues[that.getPriority()].remove(&that);

    that.finished = true;

    lock.release();
}

void Scheduler::yield() {

    if (!isThreadWaiting()) {

        return;
    }


    if (lock.tryAcquire()) {

        Thread *next = getNextThread();


        readyQueues[currentThread->getPriority()].push(currentThread);

        dispatch(*next);
    }
}

void Scheduler::block() {

    if (!isThreadWaiting()) {

        Cpu::throwException(Cpu::Exception::ILLEGAL_STATE, "Scheduler: No thread is waiting to be scheduled!");
    }

    lock.acquire();

    Thread *next = getNextThread();

    dispatch(*next);
}

void Scheduler::deblock(Thread &that) {

    lock.acquire();

    if (!initialized) {

        Cpu::throwException(Cpu::Exception::ILLEGAL_STATE, "Scheduler: 'deblock' called but scheduler is not initialized!");
    }

    readyQueues[that.getPriority()].push(&that);

    lock.release();
}

void Scheduler::dispatch(Thread &next) {

    if (!initialized) {

        Cpu::throwException(Cpu::Exception::ILLEGAL_STATE, "Scheduler: 'dispatch' called but scheduler is not initialized!");
    }

    Thread *current = currentThread;

    currentThread = &next;

    switchContext(&current->kernelContext, &next.kernelContext);
}

Thread *Scheduler::getNextThread() {

    if (!isThreadWaiting()) {
        Cpu::throwException(Cpu::Exception::ILLEGAL_STATE, "Scheduler: No thread is waiting to be scheduled!");
    }

    Util::ArrayBlockingQueue<Thread *> *currentQueue = &readyQueues[priority.getNextPriority()];

    while (currentQueue->isEmpty()) {
        currentQueue = &readyQueues[priority.getNextPriority()];
    }

    Thread *ret = currentQueue->pop();

    return ret;
}

bool Scheduler::isInitialized() {

    return initialized;
}

bool Scheduler::isThreadWaiting() {

    for (const auto &queue : readyQueues) {
        if (!queue.isEmpty()) {
            return true;
        }
    }

    return false;
}

uint32_t Scheduler::getThreadCount() {

    uint32_t count = 0;

    for (const auto &queue : readyQueues) {
        count += queue.size();
    }

    return count;
}

uint8_t Scheduler::getMaxPriority() {

    return static_cast<uint8_t>(readyQueues.length() - 1);
}

uint8_t Scheduler::changePriority(Thread &thread, uint8_t priority) {

    priority = static_cast<uint8_t>((priority > getMaxPriority()) ? (getMaxPriority()) : priority);

    lock.acquire();

    if (&thread == currentThread) {
        lock.release();

        return priority;
    }

    readyQueues[thread.getPriority()].remove(&thread);

    readyQueues[priority].push(&thread);

    lock.release();

    return priority;
}

String Scheduler::getAllProcesses(){
    String data = "";
    for (const auto &queue : readyQueues) {
        for(auto x : queue){
            // char temp[50];
            // sprintf(temp,"%d",x->getId());
    //        char *temp2;
    //        sprintf(temp2,"%d",x->getName()); 
            data += x->getName();
            data += "\n";
    //        strcat(data," ");
    //        strcat(data,temp2);
    //        strcat(data,"\n"); 
        }
        // strcat(data,"N");
    }
    return data;
}

uint32_t Scheduler::getLength(){
    return getThreadCount();
}

}