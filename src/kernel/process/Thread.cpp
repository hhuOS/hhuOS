/*
 * Copyright (C) 2017-2025 Heinrich Heine University Düsseldorf,
 * Institute of Computer Science, Department Operating Systems
 * Main developers: Christian Gesse <christian.gesse@hhu.de>, Fabian Ruhland <ruhland@hhu.de>
 * Original development team: Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schöttner
 * This project has been supported by several students.
 * A full list of integrated student theses can be found here: https://github.com/hhuOS/hhuOS/wiki/Student-theses
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
#include "Thread.h"
#include "kernel/service/MemoryService.h"
#include "lib/util/async/IdGenerator.h"
#include "lib/util/async/Runnable.h"
#include "lib/util/base/Constants.h"
#include "kernel/process/Process.h"
#include "device/cpu/Cpu.h"
#include "kernel/service/Service.h"
#include "kernel/service/ProcessService.h"
#include "kernel/process/Scheduler.h"
#include "kernel/service/CpuService.h"

extern "C" void set_tss_stack_entry(uint32_t *stackPointer) {
    Kernel::Service::getService<Kernel::CpuService>().setTssStackEntry(stackPointer);
}

extern "C" void release_scheduler_lock() {
    Kernel::Service::getService<Kernel::ProcessService>().getScheduler().unlockReadyQueue();
}

namespace Kernel {

Util::Async::IdGenerator Thread::idGenerator;

Thread::Thread(const Util::String &name, Process &parent, Util::Async::Runnable *runnable, uint32_t userInstructionPointer, uint32_t *kernelStack, uint32_t *userStack) :
        id(idGenerator.next()), name(name), parent(parent), runnable(runnable), userInstructionPointer(userInstructionPointer), kernelStack(kernelStack), userStack(userStack),
        fpuContext(static_cast<uint8_t*>(Service::getService<MemoryService>().allocateKernelMemory(512, 16))) {
    auto defaultFpuContext = Util::Address(Service::getService<ProcessService>().getScheduler().getDefaultFpuContext());
    Util::Address(fpuContext).copyRange(defaultFpuContext, 512);
}

Thread::~Thread() {
    delete reinterpret_cast<uint8_t*>(kernelStack);
    delete fpuContext;

    if (isKernelThread()) {
        delete runnable;
    }
}

Thread& Thread::createKernelThread(const Util::String &name, Process &parent, Util::Async::Runnable *runnable) {
    auto *stack = createKernelStack();
    auto *thread = new Thread(name, parent, runnable, 0, stack, nullptr);

    thread->prepareKernelStack();

    return *thread;
}

Thread& Thread::createKernelProcessThread(const Util::String &name, Util::Async::Runnable *runnable) {
    auto &processService = Service::getService<ProcessService>();
    return createKernelThread(name, processService.getKernelProcess(), runnable);
}

Thread& Thread::createUserThread(const Util::String &name, Process &parent, uint32_t eip, Util::Async::Runnable *runnable) {
    auto *kernelStack = Thread::createKernelStack();
    auto *userStack = Thread::createUserStack();
    auto *thread = new Thread(name, parent, runnable, eip, kernelStack, userStack);

    thread->prepareKernelStack();

    const auto capacity = Util::MAX_USER_STACK_SIZE / sizeof(uint32_t);
    thread->userStack[capacity - 1] = 0x00DEAD00; // Dummy return address

    // Parameters for 'kickoffUserThread' (empty space before is only used when creating a main thread)
    thread->userStack[capacity - 4] = reinterpret_cast<uint32_t>(runnable);

    return *thread;
}

Thread& Thread::createMainUserThread(const Util::String &name, Process &parent, uint32_t eip, uint32_t argc, char **argv, void *envp, uint32_t heapStartAddress) {
    auto *kernelStack = createKernelStack();
    auto *userStack = createMainUserStack();
    auto *thread = new Thread(name, parent, nullptr, eip, kernelStack, userStack);

    thread->prepareKernelStack();

    const auto capacity = Util::MAX_USER_STACK_SIZE / sizeof(uint32_t);
    thread->userStack[capacity - 1] = 0x00DEAD00; // Dummy return address

    // Parameters for 'main()'
    thread->userStack[capacity - 2] = reinterpret_cast<uint32_t>(envp);
    thread->userStack[capacity - 3] = reinterpret_cast<uint32_t>(argv);
    thread->userStack[capacity - 4] = argc;

    // Parameters for 'initMemoryManager'
    thread->userStack[capacity - 5] = heapStartAddress;

    return *thread;
}

void Thread::freeUserStack() {
    if (isKernelThread()) {
        return;
    }

    auto &userStackMemoryManager = Util::System::getAddressSpaceHeader().stackMemoryManager;
    auto &memoryService = Service::getService<MemoryService>();

    memoryService.unmap(userStack, Util::MAX_USER_STACK_SIZE / Util::PAGESIZE, 1);
    userStackMemoryManager.freeBlock(userStack);
}

void Thread::prepareKernelStack() {
    Util::Address(kernelStack).setRange(0, KERNEL_STACK_SIZE);

    const auto capacity = KERNEL_STACK_SIZE / sizeof(uint32_t);
    kernelStack[capacity - 1] = 0x0000DEAD; // Dummy return address
    kernelStack[capacity - 2] = reinterpret_cast<uint32_t>(kickoffKernelThread); // Address of 'kickoff_kernel_thread()'

    kernelStack[capacity - 3] = 0; // eax
    kernelStack[capacity - 4] = 0; // ecx
    kernelStack[capacity - 5] = 0; // edx
    kernelStack[capacity - 6] = 0; // ebx
    kernelStack[capacity - 7] = 0; // Skipped by 'popad' ('pushad' pushes esp here)
    kernelStack[capacity - 8] = 0; // ebp
    kernelStack[capacity - 9] = 0; // esi
    kernelStack[capacity - 10] = 0; // edi

    kernelStack[capacity - 11] = 0x202; // eflags (interrupts enabled)

    kernelStack[capacity - 12] = static_cast<uint16_t>(Device::Cpu::SegmentSelector(Device::Cpu::Ring0, 2)); // gs
    kernelStack[capacity - 13] = static_cast<uint16_t>(Device::Cpu::SegmentSelector(Device::Cpu::Ring0, 2)); // fs
    kernelStack[capacity - 14] = static_cast<uint16_t>(Device::Cpu::SegmentSelector(Device::Cpu::Ring0, 2)); // es
    kernelStack[capacity - 15] = static_cast<uint16_t>(Device::Cpu::SegmentSelector(Device::Cpu::Ring0, 2)); // ds

    oldStackPointer = kernelStack + (capacity - 15);
}

void Thread::kickoffKernelThread() {
    auto &scheduler = Kernel::Service::getService<Kernel::ProcessService>().getScheduler();
    scheduler.setInitialized();

    auto &thread = scheduler.getCurrentThread();

    if (thread.isKernelThread()) {
        thread.runnable->run();
        scheduler.exit();
    } else {
        thread.switchToUserMode();
    }
}

void Thread::switchToUserMode() {
    const auto kernelStackCapacity = KERNEL_STACK_SIZE / sizeof(uint32_t);
    const auto userStackCapacity = Util::MAX_USER_STACK_SIZE / sizeof(uint32_t);

    kernelStack[kernelStackCapacity - 1] = 0x00DEAD00; // Dummy return address;

    kernelStack[kernelStackCapacity - 2] = static_cast<uint16_t>(Device::Cpu::SegmentSelector(Device::Cpu::Ring3, 4)); // ss = user data segment
    kernelStack[kernelStackCapacity - 3] = reinterpret_cast<uint32_t>(userStack + (userStackCapacity - 5)); // esp for user stack (leave room for 'main()' arguments)
    kernelStack[kernelStackCapacity - 4] = 0x202; // eflags (interrupts enabled)
    kernelStack[kernelStackCapacity - 5] = static_cast<uint16_t>(Device::Cpu::SegmentSelector(Device::Cpu::Ring3, 3)); // cs = user code segment

    kernelStack[kernelStackCapacity - 6] = userInstructionPointer; // Address of user function to start

    oldStackPointer = kernelStack + (kernelStackCapacity - 6);

    asm volatile ( "cli" ); // Interrupts are enabled again by iret in start_user_thread -> Do not use Device::Cpu::disableInterrupts() here
    Device::Cpu::writeSegmentRegister(Device::Cpu::DS, Device::Cpu::SegmentSelector(Device::Cpu::Ring3, 4));
    Device::Cpu::writeSegmentRegister(Device::Cpu::ES, Device::Cpu::SegmentSelector(Device::Cpu::Ring3, 4));
    Device::Cpu::writeSegmentRegister(Device::Cpu::FS, Device::Cpu::SegmentSelector(Device::Cpu::Ring3, 4));
    Device::Cpu::writeSegmentRegister(Device::Cpu::GS, Device::Cpu::SegmentSelector(Device::Cpu::Ring3, 4));

    startUserThread(oldStackPointer);
}

void Thread::startFirstThread(const Thread &thread) {
    startKernelThread(thread.oldStackPointer);
}

void Thread::switchThread(Thread &current, const Thread &next) {
    Service::getService<MemoryService>().switchAddressSpace(next.parent.getAddressSpace());
    switchThread(&current.oldStackPointer, next.oldStackPointer, next.kernelStack + (KERNEL_STACK_SIZE / sizeof(uint32_t)));
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

bool Thread::isKernelThread() const {
    return userStack == nullptr;
}

void Thread::join() {
    Service::getService<ProcessService>().getScheduler().join(*this);
}

uint32_t* Thread::createKernelStack() {
    auto &memoryService = Kernel::Service::getService<Kernel::MemoryService>();
    return static_cast<uint32_t*>(memoryService.allocateKernelMemory(KERNEL_STACK_SIZE, 16));
}

uint32_t* Thread::createUserStack() {
    auto &stackMemoryManager = Util::System::getAddressSpaceHeader().stackMemoryManager;
    return static_cast<uint32_t*>(stackMemoryManager.allocateBlock());
}

uint32_t* Thread::createMainUserStack() {
    return reinterpret_cast<uint32_t*>(Util::MAIN_STACK_START_ADDRESS);
}

void Thread::startKernelThread([[maybe_unused]] uint32_t *oldStackPointer) {
    asm volatile (
            "mov 4(%esp), %esp;" // First parameter -> load 'oldStackPointer'

            // Load registers from prepared stack
            "pop %ds;"
            "pop %es;"
            "pop %fs;"
            "pop %gs;"
            "popfl;"
            "popal;"

            "call release_scheduler_lock;"

            "ret;"
            );
}

void Thread::startUserThread([[maybe_unused]] uint32_t *oldStackPointer) {
    asm volatile (
            "mov 4(%esp), %esp;" // First parameter -> load 'oldStackPointer'
            "iret;" // Switch to user mode
            );
}

void Thread::switchThread([[maybe_unused]] uint32_t **oldStackPointer, [[maybe_unused]] uint32_t *newStackPointer, [[maybe_unused]] uint32_t *stackEnd) {
    asm volatile (
            // Save registers of current thread (Total size of all registers: 32 + 4 + 16 = 52 bytes)
            "pushal;" // Pushes 32 bytes (8 registers * 4 bytes)
            "pushfl;" // Pushes 4 bytes (eflags)
            // Push segment registers (4 * 4 bytes)
            "push %gs;"
            "push %fs;"
            "push %es;"
            "push %ds;"

            // Save stack pointer in first parameter 'currentStackPointer'
            "mov 56(%esp), %eax;" // 52 bytes of saved registers + 4 bytes for first parameter
            "mov %esp, (%eax);"

            // Set TSS stack entry using third parameter 'nextStackEndPointer'
            "push 64(%esp);" // 52 bytes of saved registers + 3 * 4 bytes for third parameter
            "call set_tss_stack_entry;"
            "add $4, %esp;"

            // Load registers of next thread using second parameter 'nextStackPointer'
            "mov 60(%esp), %esp;" // 52 bytes of saved registers + 2 * 4 bytes for second parameter
            "pop %ds;"
            "pop %es;"
            "pop %fs;"
            "pop %gs;"
            "popfl;"
            "popal;"

            "call release_scheduler_lock;"
            "ret;"
            );
}

}