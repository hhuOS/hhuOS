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
#include <devices/Pit.h>
#include <kernel/services/SoundService.h>
#include <devices/sound/SoundBlaster/SoundBlaster.h>
#include "kernel/threads/Scheduler.h"
#include "Scheduler.h"


extern "C" {
    void startThread(Context* first);
    void switchContext(Context **current, Context **next);
    void setSchedInit();
    void schedulerYield();
    void checkIoBuffers();
}

void checkIoBuffers() {
    Util::ArrayList<IODevice*> &ioDevices = Scheduler::getInstance()->ioDevices;

    for(uint32_t i = 0; i < ioDevices.size(); i++) {
        if(ioDevices.get(i)->checkForData()) {
            ioDevices.get(i)->trigger();
        }
    }
}

Scheduler* Scheduler::scheduler = nullptr;

void schedulerYield() {

    Scheduler::getInstance()->yield();
}

Scheduler::Scheduler() : initialized(false) {

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

void Scheduler::schedule() {

    Thread* first;

    if (!isThreadWaiting()) {

        Cpu::throwException(Cpu::Exception::ILLEGAL_STATE);
    }

    Cpu::disableInterrupts();

    first = readyQueue.pop();

    currentThread = first;

    initialized = true;

    setSchedInit();

    Pit::getInstance()->setYieldable(this);

    startThread(currentThread->context);
}

void Scheduler::ready(Thread& that) {

    Cpu::disableInterrupts();

    readyQueue.push(&that);

    Cpu::enableInterrupts();
}

void Scheduler::exit() {

    if (!initialized) {

        Cpu::throwException(Cpu::Exception::ILLEGAL_STATE);
    }

    Cpu::disableInterrupts();
    
    if (!isThreadWaiting()) {

        Cpu::throwException(Cpu::Exception::ILLEGAL_STATE);
    }

    Thread* next = readyQueue.pop();
    
    dispatch (*next);
}

void Scheduler::kill(Thread& that) {

    if (!initialized) {

        Cpu::throwException(Cpu::Exception::ILLEGAL_STATE);
    }

    Cpu::disableInterrupts();

    readyQueue.remove(&that);

    Cpu::enableInterrupts();
}

void Scheduler::yield() {

    if (!initialized) {

        Cpu::throwException(Cpu::Exception::ILLEGAL_STATE);
    }

    if (!isThreadWaiting()) {

        return;
    }

    Cpu::disableInterrupts();

    Thread *next = readyQueue.pop();

    readyQueue.push(active());

    dispatch(*next);
}

void Scheduler::block() {

    if (!initialized) {

        Cpu::throwException(Cpu::Exception::ILLEGAL_STATE);
    }

    Thread* next;
    
    Cpu::disableInterrupts();
    
    if (!isThreadWaiting()) {

        Cpu::throwException(Cpu::Exception::ILLEGAL_STATE);
    }
    
    next = readyQueue.pop();
    
    dispatch (*next);
}

void Scheduler::deblock(Thread &that) {

    if (!initialized) {

        Cpu::throwException(Cpu::Exception::ILLEGAL_STATE);
    }

    Cpu::disableInterrupts();

    readyQueue.push(&that);

    Cpu::enableInterrupts();
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


