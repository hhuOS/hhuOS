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

#include "kernel/service/InterruptService.h"
#include "lib/util/hardware/CpuId.h"
#include "Cpu.h"
#include "Fpu.h"
#include "device/cpu/Fpu.h"
#include "kernel/log/Log.h"
#include "kernel/process/Thread.h"
#include "kernel/service/SchedulerService.h"
#include "lib/util/collection/Array.h"
#include "kernel/interrupt/InterruptVector.h"
#include "kernel/service/Service.h"

namespace Kernel {
struct InterruptFrame;
}  // namespace Kernel

namespace Device {

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

    if (isFxsrAvailable()) {
        LOG_INFO("FXSR support detected -> Using FXSAVE/FXRSTR for FPU context switching");

        auto features = Util::Hardware::CpuId::getCpuFeatures();
        if (features.contains(Util::Hardware::CpuId::MMX)) {
            LOG_INFO("MMX support detected");
        }

        if (features.contains(Util::Hardware::CpuId::SSE)) {
            LOG_INFO("SSE support detected -> Activating OSFXSR and OSXMMEXCPT");
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
        LOG_INFO("FXSR is not supported -> Falling back to FNSAVE/FRSTR for FPU context switching");
        asm volatile (
                "fninit;"
                "fnsave (%0);"
                : :
                "r"(defaultFpuContext)
                );
    }
}

bool Fpu::isAvailable() {
    if (Util::Hardware::CpuId::getCpuFeatures().contains(Util::Hardware::CpuId::FPU)) {
        return true;
    }

    auto cr0 = Cpu::readCr0();
    if (cr0 & Cpu::X87_FPU_EMULATION) {
        return false;
    }

    if (!(cr0 & Cpu::EXTENSION_TYPE)) {
        return false;
    }

    return probeFpu();
}

bool Fpu::isFxsrAvailable() {
    return Util::Hardware::CpuId::getCpuFeatures().contains(Util::Hardware::CpuId::FXSR);
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

void Fpu::switchContext() const {
    auto &schedulerService = Kernel::Service::getService<Kernel::SchedulerService>();
    auto &currentThread = schedulerService.getCurrentThread();
    auto *lastFpuThread = schedulerService.getLastFpuThread();

    if (fxsrAvailable) {
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
    } else {
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