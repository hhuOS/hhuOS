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

#include "lib/util/hardware/CpuId.h"
#include "Cpu.h"
#include "Fpu.h"
#include "device/cpu/Fpu.h"
#include "kernel/log/Log.h"
#include "kernel/process/Thread.h"
#include "lib/util/collection/Array.h"
#include "kernel/service/Service.h"
#include "kernel/service/ProcessService.h"
#include "kernel/process/Scheduler.h"

namespace Device {

Fpu::Fpu(uint8_t *defaultFpuContext) {
    disarmFpuMonitor();

    // Make sure FPU emulation is disabled
    Device::Cpu::writeCr0(Device::Cpu::readCr0() & ~Device::Cpu::X87_FPU_EMULATION);

    if (isFxsrAvailable()) {
        LOG_INFO("FXSR support detected -> Using FXSAVE/FXRSTR for FPU context switching");
        fxsrAvailable = true;

        auto cpuInfo = Util::Hardware::CpuId::getCpuInfo();
        if (cpuInfo.features & Util::Hardware::CpuId::MMX) {
            LOG_INFO("MMX support detected");
        }

        if (cpuInfo.features & Util::Hardware::CpuId::SSE) {
            LOG_INFO("SSE support detected -> Activating OSFXSR and OSXMMEXCPT");
            Device::Cpu::writeCr4(Device::Cpu::readCr4() | Device::Cpu::OS_FXSR | Device::Cpu::OS_XMM_EXCEPTIONS);
        }

        asm volatile (
                "fninit;"
                "fxsave %0;"
                : "=m"(*defaultFpuContext)
                );
    } else {
        LOG_INFO("FXSR is not supported -> Falling back to FNSAVE/FRSTR for FPU context switching");
        asm volatile (
                "fninit;"
                "fnsave %0;"
                : "=m"(*defaultFpuContext)
                );
    }
}

bool Fpu::isAvailable() {
    auto cpuInfo = Util::Hardware::CpuId::getCpuInfo();
    if (cpuInfo.features & Util::Hardware::CpuId::FPU) {
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
    auto cpuInfo = Util::Hardware::CpuId::getCpuInfo();
    return (cpuInfo.features & Util::Hardware::CpuId::FXSR) != 0;
}

bool Fpu::probeFpu() {
    uint16_t fpuStatus = 0x1797;
    asm volatile (
            "mov %%cr0, %%eax;"
            "and $0xfffffff3, %%eax;"
            "mov %%eax, %%cr0;"
            "fninit;"
            "fnstsw %0;"
            : "=m"(fpuStatus)
            : :
            "eax"
            );

    return fpuStatus == 0;
}

void Fpu::switchContext() const {
    auto &scheduler = Kernel::Service::getService<Kernel::ProcessService>().getScheduler();
    auto &currentThread = scheduler.getCurrentThread();
    auto *lastFpuThread = scheduler.getLastFpuThread();

    if (fxsrAvailable) {
        if (lastFpuThread != nullptr) {
            asm volatile (
                    "fxsave %0;"
                    : "=m"(*lastFpuThread->getFpuContext())
                    );
        }

        asm volatile (
                "fxrstor %0"
                : :
                "m"(*currentThread.getFpuContext())
                );
    } else {
        if (lastFpuThread != nullptr) {
            asm volatile (
                    "fnsave %0;"
                    : "=m"(*lastFpuThread->getFpuContext())
                    );
        }

        asm volatile (
                "frstor %0"
                : :
                "m"(*currentThread.getFpuContext())
                );
    }
}

void Fpu::armFpuMonitor() {
    Device::Cpu::writeCr0(Device::Cpu::readCr0() | Device::Cpu::MONITOR_COPROCESSOR);
}

void Fpu::disarmFpuMonitor() {
    Device::Cpu::writeCr0(Device::Cpu::readCr0() & ~Device::Cpu::MONITOR_COPROCESSOR);
}

}