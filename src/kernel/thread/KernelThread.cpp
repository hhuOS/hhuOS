/*
 * Copyright (C) 2019 Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schoettner
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

#include "KernelThread.h"

extern "C" {
    void interrupt_return();
    void kickoff();
}

namespace Kernel {


KernelThread::KernelThread() : Thread(), kernelStack(STACK_SIZE_DEFAULT) {
    init();
}

KernelThread::KernelThread(const String &name) : Thread(name), kernelStack(STACK_SIZE_DEFAULT) {
    init();
}

KernelThread::KernelThread(const String &name, uint8_t priority) : Thread(name, priority), kernelStack(STACK_SIZE_DEFAULT) {
    init();
}

void KernelThread::init() {

    auto *esp = (uint32_t *) kernelStack.getStart();

    esp -= (sizeof(InterruptFrame) / 4);

    interruptFrame = (InterruptFrame *) esp;

    esp -= (sizeof(Context) / 4);

    kernelContext = (Context *) esp;

    kernelContext->eip = (uint32_t) interrupt_return;

    interruptFrame->cs = 0x08;
    interruptFrame->fs = 0x10;
    interruptFrame->gs = 0x10;
    interruptFrame->ds = 0x10;
    interruptFrame->es = 0x10;
    interruptFrame->ss = 0x10;

    interruptFrame->ebx = 0;
    interruptFrame->esi = 0;
    interruptFrame->edi = 0;
    interruptFrame->esp = 0; // POPAD Skips ESP
    interruptFrame->ebp = (uint32_t) kernelStack.getStart();
    interruptFrame->uesp = (uint32_t) kernelStack.getStart();
    interruptFrame->eflags = 0x200;
    interruptFrame->eip = (uint32_t) kickoff;
}

Thread::Stack &KernelThread::getUserStack() {
    return kernelStack;
}

Thread::Stack &KernelThread::getKernelStack() {
    return kernelStack;
}

}