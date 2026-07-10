/*
 * Copyright (C) 2017-2026 Heinrich Heine University Düsseldorf,
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
    Cpu::writeCr0(Cpu::readCr0() & ~Cpu::X87_FPU_EMULATION);
    Cpu::writeCr0(Cpu::readCr0() | Cpu::MONITOR_COPROCESSOR);

    if (isFxsrAvailable()) {
        LOG_INFO("FXSR support detected -> Using FXSAVE/FXRSTR for FPU context switching");
        fxsrAvailable = true;

        auto cpuInfo = Util::Hardware::CpuId::getCpuInfo();
        if (cpuInfo.features & Util::Hardware::CpuId::MMX) {
            LOG_INFO("MMX support detected");
        }

        if (cpuInfo.features & Util::Hardware::CpuId::SSE) {
            LOG_INFO("SSE support detected -> Activating OSFXSR and OSXMMEXCPT");
            Cpu::writeCr4(Cpu::readCr4() | Cpu::OS_FXSR | Cpu::OS_XMM_EXCEPTIONS);
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

void Fpu::saveContext(const Kernel::Thread &thread) {
    if (fxsrAvailable) {
        asm volatile (
                "fxsave %0;"
                : "=m"(*thread.getFpuContext())
                );
    } else {
        asm volatile (
                "fnsave %0;"
                : "=m"(*thread.getFpuContext())
                );
    }
}

void Fpu::restoreContext(const Kernel::Thread &thread) {
    if (fxsrAvailable) {
        asm volatile (
                "fxrstor %0;"
                : "=m"(*thread.getFpuContext())
                );
    } else {
        asm volatile (
                "frstor %0;"
                : "=m"(*thread.getFpuContext())
                );
    }
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

}