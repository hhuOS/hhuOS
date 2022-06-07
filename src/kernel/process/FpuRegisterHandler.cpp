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

#include "FpuRegisterHandler.h"
#include "kernel/interrupt/InterruptDispatcher.h"
#include "kernel/system/System.h"

void Kernel::FpuRegisterHandler::plugin() {
    InterruptDispatcher::getInstance().assign(InterruptDispatcher::DEVICE_NOT_AVAILABLE, *this);
}

void Kernel::FpuRegisterHandler::trigger(Kernel::InterruptFrame &frame) {
    auto &schedulerService = System::getService<SchedulerService>();
    schedulerService.lockScheduler();

    // Disable FPU monitoring (will be enabled by scheduler at next thread switch)
    asm volatile (
            "mov %%cr0, %%eax;"
            "and $0xfffffff7, %%eax;"
            "mov %%eax, %%cr0;"
            : : :
            "%eax"
            );

    auto &currentThread = schedulerService.getCurrentThread();
    if (&currentThread == lastFpuThread) {
        schedulerService.unlockScheduler();
        return;
    }

    if (lastFpuThread != nullptr) {
        asm volatile (
                "fxsave (%0)"
                : :
                "r"(lastFpuThread->getFpuContext())
                );
    }

    if (currentThread.hasInitializedFpu()) {
        asm volatile(
                "fxrstor (%0)"
                : :
                "r"(currentThread.getFpuContext())
                );
    } else {
        currentThread.initializeFpu();
    }

    lastFpuThread = &currentThread;
    schedulerService.unlockScheduler();
}

void Kernel::FpuRegisterHandler::checkTerminatedThread(Thread &thread) {
    Util::Async::Atomic<uint32_t> wrapper(reinterpret_cast<uint32_t&>(lastFpuThread));
    wrapper.compareAndSet(reinterpret_cast<uint32_t>(&thread), 0);
}

void Kernel::FpuRegisterHandler::armFpuMonitor() {
    asm volatile (
            "mov %%cr0, %%eax;"
            "or $0xa, %%eax;"
            "mov %%eax, %%cr0;"
            : : :
            "%eax"
            );
}
