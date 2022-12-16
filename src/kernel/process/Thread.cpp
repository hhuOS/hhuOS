/*
 * Copyright (C) 2018-2022 Heinrich-Heine-Universitaet Duesseldorf,
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

#include "lib/util/memory/Address.h"
#include "lib/util/memory/operators.h"
#include "kernel/system/System.h"
#include "kernel/paging/MemoryLayout.h"
#include "kernel/paging/Paging.h"
#include "asm_interface.h"
#include "Thread.h"
#include "kernel/process/ThreadState.h"
#include "kernel/service/MemoryService.h"
#include "kernel/service/SchedulerService.h"
#include "lib/util/async/IdGenerator.h"
#include "lib/util/async/Runnable.h"
#include "lib/util/memory/Constants.h"

void kickoff() {
    Kernel::System::getService<Kernel::SchedulerService>().kickoffThread();
}

namespace Kernel {

Util::Async::IdGenerator<uint32_t> Thread::idGenerator;

Thread::Thread(const Util::Memory::String &name, Process &parent, Util::Async::Runnable *runnable, Thread::Stack *kernelStack, Thread::Stack *userStack) :
        id(idGenerator.next()), name(name), parent(parent), runnable(runnable), kernelStack(kernelStack), userStack(userStack),
        interruptFrame(*reinterpret_cast<InterruptFrame*>(kernelStack->getStart() - sizeof(InterruptFrame))),
        kernelContext(reinterpret_cast<Context*>(kernelStack->getStart() - sizeof(InterruptFrame) - sizeof(Context))),
        fpuContext(static_cast<uint8_t*>(System::getService<MemoryService>().allocateKernelMemory(512, 16))) {
    auto source = Util::Memory::Address<uint32_t>(System::getService<SchedulerService>().getDefaultFpuContext());
    Util::Memory::Address<uint32_t>(fpuContext).copyRange(source, 512);
}

Thread::~Thread() {
    // Do not delete user stack, as it is hard coded
    // TODO: Once a process can have multiple user threads, this needs to be revised
    delete kernelStack;
    delete fpuContext;
    delete runnable;
}

Thread& Thread::createKernelThread(const Util::Memory::String &name, Process &parent, Util::Async::Runnable *runnable) {
    auto *stack = Stack::createKernelStack(DEFAULT_STACK_SIZE);
    auto *thread = new Thread(name, parent, runnable, stack, stack);

    thread->kernelContext->eip = reinterpret_cast<uint32_t>(interrupt_return);

    thread->interruptFrame.cs = 0x08;
    thread->interruptFrame.fs = 0x10;
    thread->interruptFrame.gs = 0x10;
    thread->interruptFrame.ds = 0x10;
    thread->interruptFrame.es = 0x10;
    thread->interruptFrame.ss = 0x10;

    thread->interruptFrame.ebp = reinterpret_cast<uint32_t>(stack->getStart());
    thread->interruptFrame.uesp = reinterpret_cast<uint32_t>(stack->getStart());
    thread->interruptFrame.eflags = 0x200;
    thread->interruptFrame.eip = reinterpret_cast<uint32_t>(kickoff);

    return *thread;
}

Thread& Thread::createUserThread(const Util::Memory::String &name, Process &parent, uint32_t eip, Util::Async::Runnable *runnable) {
    auto *kernelStack = Stack::createKernelStack(DEFAULT_STACK_SIZE);
    auto *userStack = Stack::createUserStack(DEFAULT_STACK_SIZE);
    auto *thread = new Thread(name, parent, nullptr, kernelStack, userStack);

    thread->kernelContext->eip = reinterpret_cast<uint32_t>(interrupt_return);

    thread->interruptFrame.cs = 0x1b;
    thread->interruptFrame.fs = 0x23;
    thread->interruptFrame.gs = 0x23;
    thread->interruptFrame.ds = 0x23;
    thread->interruptFrame.es = 0x23;
    thread->interruptFrame.ss = 0x23;

    thread->interruptFrame.ebp = reinterpret_cast<uint32_t>(userStack->getStart() - 8);
    thread->interruptFrame.uesp = reinterpret_cast<uint32_t>(userStack->getStart() - 8);
    thread->interruptFrame.eflags = 0x200;
    thread->interruptFrame.eip = reinterpret_cast<uint32_t>(eip);

    *(reinterpret_cast<uint32_t*>(userStack->getStart()) - 1) = reinterpret_cast<uint32_t>(runnable);

    return *thread;
}

Thread& Thread::createMainUserThread(const Util::Memory::String &name, Process &parent, uint32_t eip, uint32_t argc, char **argv, void *envp, uint32_t heapStartAddress) {
    auto *kernelStack = Stack::createKernelStack(DEFAULT_STACK_SIZE);
    auto *userStack = Stack::createMainUserStack();
    auto *thread = new Thread(name, parent, nullptr, kernelStack, userStack);

    thread->kernelContext->eip = reinterpret_cast<uint32_t>(interrupt_return);

    thread->interruptFrame.cs = 0x1b;
    thread->interruptFrame.fs = 0x23;
    thread->interruptFrame.gs = 0x23;
    thread->interruptFrame.ds = 0x23;
    thread->interruptFrame.es = 0x23;
    thread->interruptFrame.ss = 0x23;

    thread->interruptFrame.eax = argc;
    thread->interruptFrame.ebx = reinterpret_cast<uint32_t>(argv);
    thread->interruptFrame.ecx = reinterpret_cast<uint32_t>(envp);
    thread->interruptFrame.edx = heapStartAddress;
    thread->interruptFrame.ebp = reinterpret_cast<uint32_t>(userStack->getStart());
    thread->interruptFrame.uesp = reinterpret_cast<uint32_t>(userStack->getStart());
    thread->interruptFrame.eflags = 0x200;
    thread->interruptFrame.eip = eip;

    return *thread;
}

uint32_t Thread::getId() const {
    return id;
}

Util::Memory::String Thread::getName() const {
    return name;
}

Context* Thread::getContext() const {
    return kernelContext;
}

void Thread::run() {
    runnable->run();
}

Process& Thread::getParent() const {
    return parent;
}

uint8_t *Thread::getFpuContext() const {
    return fpuContext;
}

void Thread::join() {
    auto &schedulerService = System::getService<SchedulerService>();
    joinLock.acquire();
    joinList.add(&schedulerService.getCurrentThread());
    joinLock.release();
    schedulerService.block();
}

void Thread::unblockJoinList() {
    auto &schedulerService = System::getService<SchedulerService>();
    joinLock.acquire();
    for (auto *thread : joinList) {
        schedulerService.unblock(*thread);
    }
    joinLock.release();
}

Thread::Stack::Stack(uint8_t *stack, uint32_t size) : stack(stack), size(size) {
    Util::Memory::Address<uint32_t>(stack).setRange(0, size);

    this->stack[0] = 0x44; // D
    this->stack[1] = 0x41; // A
    this->stack[2] = 0x45; // E
    this->stack[3] = 0x44; // D
}

Thread::Stack::~Stack() {
    delete[] stack;
}

uint8_t* Thread::Stack::getStart() const {
    return &stack[size];
}

Thread::Stack* Thread::Stack::createKernelStack(uint32_t size) {
    auto &memoryService = Kernel::System::getService<Kernel::MemoryService>();
    return new Stack(static_cast<uint8_t*>(memoryService.allocateKernelMemory(size, 16)), size);
}

Thread::Stack* Thread::Stack::createUserStack(uint32_t size) {
    auto &memoryService = Kernel::System::getService<Kernel::MemoryService>();
    return new Stack(static_cast<uint8_t*>(memoryService.allocateUserMemory(size, 16)), size);
}

Thread::Stack* Thread::Stack::createMainUserStack() {
    return new (reinterpret_cast<void*>(Util::Memory::USER_SPACE_STACK_INSTANCE_ADDRESS)) Stack(reinterpret_cast<uint8_t*>(MemoryLayout::KERNEL_START - Paging::PAGESIZE), Paging::PAGESIZE - 16);
}

}