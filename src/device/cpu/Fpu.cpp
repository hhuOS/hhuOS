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

#include "kernel/service/InterruptService.h"
#include "kernel/system/System.h"
#include "lib/util/cpu/CpuId.h"
#include "Cpu.h"
#include "Fpu.h"
#include "device/cpu/Fpu.h"
#include "kernel/interrupt/InterruptDispatcher.h"
#include "kernel/log/Logger.h"
#include "kernel/process/Thread.h"
#include "kernel/service/SchedulerService.h"
#include "lib/util/async/Atomic.h"
#include "lib/util/data/Array.h"

namespace Kernel {
struct InterruptFrame;
}  // namespace Kernel

namespace Device {

Kernel::Logger Fpu::log = Kernel::Logger::get("FPU");

Fpu::Fpu(const uint8_t *defaultFpuContext) {
    disarmFpuMonitor();

    // Make sure FPU emulation is disabled
    asm volatile (
            "mov %%cr0, %%eax;"
            "and $0xfffffffb, %%eax;"
            "mov %%eax, %%cr0;"
            : : :
            "eax"
            );

    if (Device::Fpu::isFxsrAvailable()) {
        log.info("FXSR support detected -> Using FXSAVE/FXRSTR for FPU context switching");

        auto features = Util::Cpu::CpuId::getCpuFeatures();
        if (features.contains(Util::Cpu::CpuId::MMX)) {
            log.info("MMX support detected");
        }

        if (features.contains(Util::Cpu::CpuId::SSE)) {
            log.info("SSE support detected -> Activating OSFXSR and OSXMMEXCPT");
            asm volatile (
                    "mov %%cr4, %%eax;"
                    "or $0x00000600, %%eax;"
                    "mov %%eax, %%cr4;"
                    : : :
                    "eax"
                    );
        }

        asm volatile (
                "fninit;"
                "fxsave (%0);"
                : :
                "r"(defaultFpuContext)
                );
    } else {
        log.info("FXSR is not supported -> Falling back to FNSAVE/FRSTR for FPU context switching");
        asm volatile (
                "fninit;"
                "fnsave (%0);"
                : :
                "r"(defaultFpuContext)
                );
    }
}

void Fpu::plugin() {
    Kernel::System::getService<Kernel::InterruptService>().assignInterrupt(Kernel::InterruptDispatcher::DEVICE_NOT_AVAILABLE, *this);
}

void Fpu::trigger(const Kernel::InterruptFrame &frame) {
    auto &schedulerService = Kernel::System::getService<Kernel::SchedulerService>();
    schedulerService.lockScheduler();

    // Disable FPU monitoring (will be enabled by scheduler at next thread switch)
    disarmFpuMonitor();

    auto &currentThread = schedulerService.getCurrentThread();
    if (&currentThread == lastFpuThread) {
        schedulerService.unlockScheduler();
        return;
    }

    if (fxsrAvailable) {
        switchContext(currentThread);
    } else {
        switchContextFpuOnly(currentThread);
    }

    lastFpuThread = &currentThread;
    schedulerService.unlockScheduler();
}

void Fpu::checkTerminatedThread(Kernel::Thread &thread) {
    Util::Async::Atomic<uint32_t> wrapper(reinterpret_cast<uint32_t&>(lastFpuThread));
    wrapper.compareAndSet(reinterpret_cast<uint32_t>(&thread), 0);
}

bool Fpu::isAvailable() {
    if (Util::Cpu::CpuId::getCpuFeatures().contains(Util::Cpu::CpuId::FPU)) {
        return true;
    }

    auto cr0 = Cpu::readCr0();
    if (cr0.contains(Cpu::X87_FPU_EMULATION)) {
        return false;
    }

    if (!cr0.contains(Cpu::EXTENSION_TYPE)) {
        return false;
    }

    return probeFpu();
}

bool Fpu::isFxsrAvailable() {
    return Util::Cpu::CpuId::getCpuFeatures().contains(Util::Cpu::CpuId::FXSR);
}

bool Fpu::probeFpu() {
    uint16_t fpuStatus = 0x1797;
    asm volatile (
            "mov %%cr0, %%eax;"
            "and $0xfffffff3, %%eax;"
            "mov %%eax, %%cr0;"
            "fninit;"
            "fnstsw (%0);"
            : :
            "r"(&fpuStatus)
            :
            "eax"
            );

    return fpuStatus == 0;
}

void Fpu::switchContext(Kernel::Thread &currentThread) {
    if (lastFpuThread != nullptr) {
        asm volatile (
                "fxsave (%0)"
                : :
                "r"(lastFpuThread->getFpuContext())
                );
    }

    asm volatile(
            "fxrstor (%0)"
            : :
            "r"(currentThread.getFpuContext())
            );
}

void Fpu::switchContextFpuOnly(Kernel::Thread &currentThread) {
    if (lastFpuThread != nullptr) {
        asm volatile (
                "fnsave (%0)"
                : :
                "r"(lastFpuThread->getFpuContext())
                );
    }

    asm volatile(
            "frstor (%0)"
            : :
            "r"(currentThread.getFpuContext())
            );
}

void Fpu::armFpuMonitor() {
    asm volatile (
            "mov %%cr0, %%eax;"
            "or $0xa, %%eax;"
            "mov %%eax, %%cr0;"
            : : :
            "%eax"
            );
}

void Fpu::disarmFpuMonitor() {
    asm volatile (
            "mov %%cr0, %%eax;"
            "and $0xfffffff7, %%eax;"
            "mov %%eax, %%cr0;"
            : : :
            "%eax"
            );
}

}