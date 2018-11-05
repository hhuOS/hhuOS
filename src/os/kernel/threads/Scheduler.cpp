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

#include <kernel/cpu/Cpu.h>
#include <lib/libc/printf.h>
#include <kernel/interrupts/Pic.h>
#include <kernel/Kernel.h>
#include <kernel/services/SerialService.h>
#include <devices/timer/Pit.h>
#include <kernel/services/SoundService.h>
#include <devices/sound/SoundBlaster/SoundBlaster.h>
#include <lib/system/SystemCall.h>
#include "kernel/threads/Scheduler.h"
#include "Scheduler.h"


extern "C" {
    void startThread(Context* first);
    void switchContext(Context **current, Context **next);
    void setSchedInit();
    void releaseSchedulerLock();
}

Scheduler* Scheduler::scheduler = nullptr;

void releaseSchedulerLock() {
    Scheduler::getInstance()->lock.release();
}

Scheduler::Scheduler() : initialized(false) {
    SystemCall::registerSystemCall(SystemCall::SCHEDULER_YIELD, [](){Scheduler::getInstance()->yield();});
    SystemCall::registerSystemCall(SystemCall::SCHEDULER_BLOCK, [](){Scheduler::getInstance()->block();});
}

Scheduler *Scheduler::getInstance()  {

    if(scheduler == nullptr) {

        scheduler = new Scheduler();
    }

    return scheduler;
}

void Scheduler::registerIODevice(IODevice *device) {
    ioDevices.add(device);
}

void Scheduler::startUp() {

    lock.acquire();

    if (!isThreadWaiting()) {

        Cpu::throwException(Cpu::Exception::ILLEGAL_STATE);
    }

    currentThread = readyQueue.pop();

    initialized = true;

    setSchedInit();

    Pit::getInstance()->setYieldable(this);

    startThread(currentThread->context);
}

void Scheduler::ready(Thread& that) {

    lock.acquire();

    readyQueue.push(&that);

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

    Thread* next = readyQueue.pop();
    
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

    readyQueue.remove(&that);

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

        Thread *next = readyQueue.pop();

        readyQueue.push(active());

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

    Thread* next = readyQueue.pop();
    
    dispatch (*next);
}

void Scheduler::deblock(Thread &that) {

    lock.acquire();

    if (!initialized) {

        Cpu::throwException(Cpu::Exception::ILLEGAL_STATE);
    }

    readyQueue.push(&that);

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

bool Scheduler::isInitialized() {

    return initialized;
}

bool Scheduler::isThreadWaiting() {

    return !readyQueue.isEmpty();
}

uint32_t Scheduler::getThreadCount() {
    return readyQueue.size();
}


