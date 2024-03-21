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

#include "lib/util/base/Address.h"
#include "lib/util/base/operators.h"

#include "kernel/memory/MemoryLayout.h"
#include "kernel/memory/Paging.h"
#include "asm_interface.h"
#include "Thread.h"
#include "kernel/process/ThreadState.h"
#include "kernel/service/MemoryService.h"
#include "kernel/service/SchedulerService.h"
#include "lib/util/async/IdGenerator.h"
#include "lib/util/async/Runnable.h"
#include "lib/util/base/Constants.h"
#include "lib/util/collection/Iterator.h"

extern "C" {
    void start_kernel_thread(uint32_t *oldStackPointer);
    void switch_thread(uint32_t **currentStackPointer, uint32_t *nextStackPointer);
}

void kickoff() {
    Kernel::Service::getService<Kernel::SchedulerService>().kickoffThread();
}

namespace Kernel {

Util::Async::IdGenerator<uint32_t> Thread::idGenerator;

Thread::Thread(const Util::String &name, Process &parent, Util::Async::Runnable *runnable, uint32_t *kernelStack, uint32_t *userStack) :
        id(idGenerator.next()), name(name), parent(parent), runnable(runnable), kernelStack(kernelStack), userStack(userStack),
        fpuContext(static_cast<uint8_t*>(Service::getService<MemoryService>().allocateKernelMemory(512, 16))) {
    auto source = Util::Address<uint32_t>(Service::getService<SchedulerService>().getDefaultFpuContext());
    Util::Address<uint32_t>(fpuContext).copyRange(source, 512);
}

Thread::~Thread() {
    delete reinterpret_cast<uint8_t*>(kernelStack);
    delete fpuContext;
    delete runnable;
}

Thread& Thread::createKernelThread(const Util::String &name, Process &parent, Util::Async::Runnable *runnable) {
    auto *stack = createKernelStack(STACK_SIZE);
    auto *thread = new Thread(name, parent, runnable, stack, nullptr);

    thread->prepareKernelStack();

    return *thread;
}

Thread& Thread::createUserThread(const Util::String &name, Process &parent, uint32_t eip, Util::Async::Runnable *runnable) {
    /*auto *kernelStack = Stack::createKernelStack(DEFAULT_STACK_SIZE);
    auto *userStack = Stack::createUserStack(DEFAULT_STACK_SIZE);
    auto *thread = new Thread(name, parent, nullptr, kernelStack, userStack);

    // thread->kernelContext->eip = reinterpret_cast<uint32_t>(interrupt_return);

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

    return *thread;*/
    return *new Thread(name, parent, nullptr, nullptr, nullptr);
}

Thread& Thread::createMainUserThread(const Util::String &name, Process &parent, uint32_t eip, uint32_t argc, char **argv, void *envp, uint32_t heapStartAddress) {
    /*auto *kernelStack = Stack::createKernelStack(DEFAULT_STACK_SIZE);
    auto *userStack = Stack::createMainUserStack();
    auto *thread = new Thread(name, parent, nullptr, kernelStack, userStack);

    // thread->kernelContext->eip = reinterpret_cast<uint32_t>(interrupt_return);

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

    return *thread;*/
    return *new Thread(name, parent, nullptr, nullptr, nullptr);
}

void Thread::prepareKernelStack() {
    Util::Address<uint32_t>(kernelStack).setRange(0, STACK_SIZE);

    const auto capacity = STACK_SIZE / sizeof(uint32_t);
    kernelStack[capacity - 1] = 0x00DEAD00; // Dummy return address
    kernelStack[capacity - 2] = reinterpret_cast<uint32_t>(kickoffKernelThread); // Address of 'kickoff_kernel_thread()'
    kernelStack[capacity - 3] = 0x202; // eflags (Interrupts enabled)

    kernelStack[capacity - 4] = 0; // eax
    kernelStack[capacity - 5] = 0; // ecx
    kernelStack[capacity - 6] = 0; // edx
    kernelStack[capacity - 7] = 0; // ebx
    kernelStack[capacity - 8] = 0; // Skipped by 'popad' ('pushad' pushes esp here)
    kernelStack[capacity - 9] = 0; // ebp
    kernelStack[capacity - 10] = 0; // esi
    kernelStack[capacity - 11] = 0; // edi

    oldStackPointer = kernelStack + (capacity - 11);
}

void Thread::kickoffKernelThread() {
    auto &schedulerService = Kernel::Service::getService<Kernel::SchedulerService>();
    schedulerService.setSchedulerInit();

    auto &thread = schedulerService.getCurrentThread();
    thread.runnable->run();

    schedulerService.exitCurrentThread();
}

void Thread::startFirstThread(const Thread &thread) {
    start_kernel_thread(thread.oldStackPointer);
}

void Thread::switchThread(Thread &current, const Thread &next) {
    switch_thread(&current.oldStackPointer, next.oldStackPointer);
}

uint32_t Thread::getId() const {
    return id;
}

Util::String Thread::getName() const {
    return name;
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
    Service::getService<SchedulerService>().join(*this);
}

Thread::Stack::Stack(uint8_t *stack, uint32_t size) : stack(stack), size(size) {
    Util::Address<uint32_t>(stack).setRange(0, size);

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

uint32_t* Thread::createKernelStack(uint32_t size) {
    auto &memoryService = Kernel::Service::getService<Kernel::MemoryService>();
    return static_cast<uint32_t*>(memoryService.allocateKernelMemory(size, 16));
}

uint32_t* Thread::createUserStack(uint32_t size) {
    auto &memoryService = Kernel::Service::getService<Kernel::MemoryService>();
    return static_cast<uint32_t*>(memoryService.allocateUserMemory(size, 16));
}

uint32_t* Thread::createMainUserStack() {
    return reinterpret_cast<uint32_t*>(MemoryLayout::KERNEL_START - Paging::PAGESIZE);
}

}