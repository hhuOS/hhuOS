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

#include "Cpu.h"
#include "Fpu.h"
#include "kernel/log/Log.h"
#include "kernel/process/Thread.h"
#include "kernel/service/Service.h"
#include "kernel/service/MemoryService.h"
#include "lib/util/hardware/CpuId.h"

namespace Kernel {
class MemoryService;
}

namespace Device {

bool Fpu::available = false;
bool Fpu::fxsrAvailable = false;
uint8_t* Fpu::defaultFpuContext = nullptr;

void Fpu::initialize() {
    available = checkExistence();
    if (!available) {
        LOG_WARN("No FPU detected! The kernel will boot, but some applications will not work...");
        return;
    }

    LOG_INFO("X87 FPU detected");
    Cpu::writeCr0((Cpu::readCr0() | Cpu::MONITOR_COPROCESSOR) & ~Cpu::X87_FPU_EMULATION);

    const auto cpuId = Util::Hardware::CpuId::getCpuInfo();

    if (cpuId.features & Util::Hardware::CpuId::MMX) {
        LOG_INFO("MMX support detected");
    }

    if (cpuId.features & Util::Hardware::CpuId::SSE) {
        LOG_INFO("SSE support detected -> Activating OSFXSR and OSXMMEXCPT");
        Cpu::writeCr4(Cpu::readCr4() | Cpu::OS_FXSR | Cpu::OS_XMM_EXCEPTIONS);
    }

    if (cpuId.features & Util::Hardware::CpuId::FXSR) {
        LOG_INFO("FXSR support detected -> Using FXSAVE/FXRSTOR for FPU context switching");
        fxsrAvailable = true;

        auto &memoryService = Kernel::Service::getService<Kernel::MemoryService>();
        defaultFpuContext = static_cast<uint8_t*>(memoryService.allocateKernelMemory(FXSAVE_SIZE, 16));
        Util::Address(defaultFpuContext).setRange(0, FXSAVE_SIZE);

        asm volatile (
                "fninit;"
                "fxsave %0;"
                : "=m"(*defaultFpuContext)
                );
    } else {
        LOG_INFO("FXSR is not supported -> Falling back to FNSAVE/FRSTOR for FPU context switching");

        auto &memoryService = Kernel::Service::getService<Kernel::MemoryService>();
        defaultFpuContext = static_cast<uint8_t*>(memoryService.allocateKernelMemory(FNSAVE_SIZE, 16));
        Util::Address(defaultFpuContext).setRange(0, FNSAVE_SIZE);

        asm volatile (
                "fninit;"
                "fnsave %0;"
                : "=m"(*defaultFpuContext)
                );
    }
}

uint8_t* Fpu::createContext() {
    if (!available) {
        return nullptr;
    }

    auto &memoryService = Kernel::Service::getService<Kernel::MemoryService>();
    const auto saveSize = fxsrAvailable ? FXSAVE_SIZE : FNSAVE_SIZE;
    auto *context = static_cast<uint8_t*>(memoryService.allocateKernelMemory(saveSize, 16));
    Util::Address(context).copyRange(defaultFpuContext, saveSize);

    return context;
}

void Fpu::saveContext(const Kernel::Thread &thread) {
    if (!available) {
        return;
    }

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
    if (!available) {
        return;
    }

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

bool Fpu::checkExistence() {
    const auto cpuInfo = Util::Hardware::CpuId::getCpuInfo();
    if (cpuInfo.features & Util::Hardware::CpuId::FPU) {
        return true;
    }

    const auto cr0 = Cpu::readCr0();
    if (cr0 & Cpu::X87_FPU_EMULATION) {
        return false;
    }

    if (!(cr0 & Cpu::EXTENSION_TYPE)) {
        return false;
    }

    return probeFpu();
}

// Code taken from: https://wiki.osdev.org/FPU
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
