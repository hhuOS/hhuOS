/*
 * Copyright (C) 2018-2021 Heinrich-Heine-Universitaet Duesseldorf,
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

#include "lib/util/async/Atomic.h"
#include "Thread.h"
#include "lib/util/memory/Address.h"
#include "asm_interface.h"
#include "kernel/system/System.h"

void kickoff() {
    Kernel::System::getService<Kernel::SchedulerService>().kickoffThread();
}

namespace Kernel {

Util::Async::IdGenerator<uint32_t> Thread::idGenerator;

Thread::Thread(const Util::Memory::String &name, Util::Async::Runnable *runnable, Thread::Stack *kernelStack, Thread::Stack *userStack) :
        id(idGenerator.next()), name(name), runnable(runnable),
        kernelStack(kernelStack), userStack(userStack),
        interruptFrame(*reinterpret_cast<InterruptFrame*>(kernelStack->getStart() - sizeof(InterruptFrame))),
        kernelContext(reinterpret_cast<Context*>(kernelStack->getStart() - sizeof(InterruptFrame) - sizeof(Context))) {}

Thread::~Thread() {
    if (kernelStack == userStack) {
        delete kernelStack;
    } else {
        delete kernelStack;
        delete userStack;
    }

    delete runnable;
}

Thread& Thread::createKernelThread(const Util::Memory::String &name, Util::Async::Runnable *runnable) {
    auto *stack = new Stack(DEFAULT_STACK_SIZE);
    auto *thread = new Thread(name, runnable, stack, stack);

    thread->kernelContext->eip = reinterpret_cast<uint32_t>(interrupt_return);

    thread->interruptFrame.cs = 0x08;
    thread->interruptFrame.fs = 0x10;
    thread->interruptFrame.gs = 0x10;
    thread->interruptFrame.ds = 0x10;
    thread->interruptFrame.es = 0x10;
    thread->interruptFrame.ss = 0x10;

    thread->interruptFrame.ebx = 0;
    thread->interruptFrame.esi = 0;
    thread->interruptFrame.edi = 0;
    thread->interruptFrame.esp = 0; // POPAD Skips ESP
    thread->interruptFrame.ebp = reinterpret_cast<uint32_t>(stack->getStart());
    thread->interruptFrame.uesp = reinterpret_cast<uint32_t>(stack->getStart());
    thread->interruptFrame.eflags = 0x200;
    thread->interruptFrame.eip = reinterpret_cast<uint32_t>(kickoff);

    return *thread;
}

Thread& Thread::createUserThread(const Util::Memory::String &name, Util::Async::Runnable *runnable) {
    auto *kernelStack = new Stack(DEFAULT_STACK_SIZE);
    auto *userStack = new Stack(DEFAULT_STACK_SIZE);
    auto *thread = new Thread(name, runnable, kernelStack, userStack);

    thread->kernelContext->eip = reinterpret_cast<uint32_t>(interrupt_return);

    thread->interruptFrame.cs = 0x1b;
    thread->interruptFrame.fs = 0x23;
    thread->interruptFrame.gs = 0x23;
    thread->interruptFrame.ds = 0x23;
    thread->interruptFrame.es = 0x23;
    thread->interruptFrame.ss = 0x23;

    thread->interruptFrame.ebx = 0;
    thread->interruptFrame.esi = 0;
    thread->interruptFrame.edi = 0;
    thread->interruptFrame.esp = 0; // POPAD Skips ESP
    thread->interruptFrame.ebp = reinterpret_cast<uint32_t>(kernelStack->getStart());
    thread->interruptFrame.uesp = reinterpret_cast<uint32_t>(kernelStack->getStart());
    thread->interruptFrame.eflags = 0x200;
    thread->interruptFrame.eip = reinterpret_cast<uint32_t>(kickoff);

    return *thread;
}

uint32_t Thread::getId() const {
    return id;
}

Util::Memory::String Thread::getName() const {
    return name;
}

Context *Thread::getContext() const {
    return kernelContext;
}

void Thread::run() {
    runnable->run();
}

Thread::Stack::Stack(uint32_t size) : stack(new uint8_t[size]), size(size) {
    Util::Memory::Address<uint32_t>(stack, size).setRange(0, size);

    this->stack[0] = 0x44; // D
    this->stack[1] = 0x41; // A
    this->stack[2] = 0x45; // E
    this->stack[3] = 0x44; // D
}

Thread::Stack::~Stack() {
    delete[] stack;
}

uint8_t* Thread::Stack::getStart() const {
    auto startAddress = reinterpret_cast<uint32_t>(&stack[size]);
    if (startAddress % 16 != 0) {
        startAddress &= -16;
    }

    return reinterpret_cast<uint8_t*>(startAddress);
}

}