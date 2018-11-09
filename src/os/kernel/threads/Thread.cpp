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

#include "kernel/threads/Thread.h"
#include "kernel/threads/Scheduler.h"


#include <cstdint>

extern "C" {
    void interrupt_return();
}

uint32_t threadCount = 0;

void kickoff () {

    Scheduler::getInstance().currentThread->run();

    Scheduler::getInstance().exit();
}

void Thread::init() {

    auto *esp = (uint32_t*) stack.getStart();

    esp -= (sizeof(InterruptFrame) / 4);

    interruptFrame = (InterruptFrame*) esp;

    esp -= (sizeof(Context) / 4);

    context = (Context*) esp;

    context->eip = (uint32_t) interrupt_return;

    interruptFrame->cs    = 0x08;
    interruptFrame->fs    = 0x10;
    interruptFrame->gs    = 0x10;
    interruptFrame->ds    = 0x10;
    interruptFrame->es    = 0x10;
    interruptFrame->ss    = 0x10;

    interruptFrame->ebx   = 0;
    interruptFrame->esi   = 0;
    interruptFrame->edi   = 0;
    interruptFrame->esp   = (uint32_t) esp;
    interruptFrame->ebp   = (uint32_t) stack.getStart();
    interruptFrame->uesp  = 0;
    interruptFrame->eflags = 0x200;
    interruptFrame->eip   = (uint32_t) kickoff;
}

Thread::Thread() : name("keylogger"), stack(STACK_SIZE_DEFAULT) {

    id = threadCount++;

    init();
}

Thread::Thread(const String &name) : name(name), stack(STACK_SIZE_DEFAULT) {

    id = threadCount++;

    init();
}

void Thread::start() {

    Scheduler::getInstance().ready(*this);
}

uint32_t Thread::getId() const {

    return id;
}

String Thread::getName() const {

    return name;
}

void Thread::yield() {

    Scheduler::getInstance().yield();
}

Thread::Stack::Stack(uint32_t size) : size(size){

    this->stack = new uint8_t[size];

    this->stack[0] = 0x44; // D
    this->stack[1] = 0x41; // A
    this->stack[2] = 0x45; // E
    this->stack[3] = 0x44; // D
}

uint8_t *Thread::Stack::getStart() {

    uint32_t startAddress = (uint32_t) &stack[size];

    if (startAddress % 16 != 0) {

        startAddress &= -16;
    }

    return (uint8_t*) startAddress;
}

Thread::Stack::~Stack() {

    delete[] stack;
}
