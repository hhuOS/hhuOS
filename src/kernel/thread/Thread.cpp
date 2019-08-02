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

#include "kernel/thread/Thread.h"
#include "kernel/thread/Scheduler.h"
#include "Thread.h"


#include <cstdint>
#include <kernel/core/Management.h>
#include "kernel/core/SystemCall.h"

extern "C" {
    void interrupt_return();
    void kickoff();
}

void kickoff() {

    Kernel::Scheduler::getInstance().getCurrentThread().run();

    Kernel::Scheduler::getInstance().exit();
}

namespace Kernel {

IdGenerator Thread::idGenerator;

Thread::Thread() {

    priority = static_cast<uint8_t>(Scheduler::getInstance().getMaxPriority() / 2);

    id = idGenerator.getId();

    name = String::format("Thread-%u", id);
}

Thread::Thread(const String &name) : name(name) {

    priority = static_cast<uint8_t>(Scheduler::getInstance().getMaxPriority() / 2);

    id = idGenerator.getId();
}

Thread::Thread(const String &name, uint8_t priority) : name(name) {

    Scheduler &scheduler = Scheduler::getInstance();

    this->priority = static_cast<uint8_t>((priority > scheduler.getMaxPriority()) ? (scheduler.getMaxPriority()) : priority);

    id = idGenerator.getId();
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

    Standard::System::Result result{};
    Standard::System::Call::execute(Standard::System::Call::SCHEDULER_YIELD, result, 0);
}

uint8_t Thread::getPriority() const {
    return priority;
}

void Thread::setPriority(uint8_t priority) {
    this->priority = Scheduler::getInstance().changePriority(*this, priority);
}

void Thread::join() const {
    while (!finished) {
        Standard::System::Result result{};
        Standard::System::Call::execute(Standard::System::Call::SCHEDULER_YIELD, result, 0);
    }
}

bool Thread::hasStarted() const {
    return started;
}

bool Thread::hasFinished() const {
    return finished;
}

InterruptFrame& Thread::getInterruptFrame() const {
    return *interruptFrame;
}

Context& Thread::getKernelContext() const {
    return *kernelContext;
}

Thread::Stack::Stack(uint32_t size) : size(size) {

    this->stack = new uint8_t[size];

    this->stack[0] = 0x44; // D
    this->stack[1] = 0x41; // A
    this->stack[2] = 0x45; // E
    this->stack[3] = 0x44; // D
}

uint8_t *Thread::Stack::getStart() {

    auto startAddress = (uint32_t) &stack[size];

    if (startAddress % 16 != 0) {

        startAddress &= -16;
    }

    return (uint8_t *) startAddress;
}

Thread::Stack::~Stack() {

    delete[] stack;
}

}