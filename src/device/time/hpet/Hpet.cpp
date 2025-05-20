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
 *
 * The HPET driver is based on a bachelor's thesis, written by Suratsch Hassan.
 * The original source code can be found here: https://git.hhu.de/bsinfo/thesis/ba-suhas102
 */

#include "Hpet.h"

#include "Timer.h"
#include "device/system/Acpi.h"
#include "kernel/service/InformationService.h"
#include "kernel/log/Log.h"
#include "kernel/service/MemoryService.h"
#include "kernel/service/InterruptService.h"
#include "device/interrupt/InterruptRequest.h"
#include "device/time/hpet/SystemTimerInterruptHandler.h"
#include "kernel/service/Service.h"
#include "lib/util/base/Panic.h"
#include "lib/util/collection/Array.h"
#include "lib/util/hardware/Acpi.h"

namespace Device {

Hpet::Hpet() {
    auto &acpi = Kernel::Service::getService<Kernel::InformationService>().getAcpi();
    if (!acpi.hasTable("HPET")) {
        Util::Panic::fire(Util::Panic::UNSUPPORTED_OPERATION, "Trying to initializeScene non-existent HPET!");
    }

    const auto &hpetTable = acpi.getTable<Util::Hardware::Acpi::Hpet>("HPET");
    auto &memoryService = Kernel::Service::getService<Kernel::MemoryService>();
    baseAddress = static_cast<uint8_t*>(memoryService.mapIO(reinterpret_cast<uint8_t*>(hpetTable.address.address), 1));

    auto capabilities = readRegister(GENERAL_CAPABILITIES_ID);
    bool bits64 = capabilities & (1 << 13);
    femtosecondsPerTick = (capabilities >> 32) & 0xffffffff;

    LOG_INFO("Vendor: [0x%04x], Revision: [%u], Comparators: [%u], Counter clock period: [%u ns], Counter size: [%s]",
             static_cast<uint32_t>((capabilities >> 16) & 0xffff),
             static_cast<uint32_t>(capabilities & 0xff),
             static_cast<uint32_t>((capabilities >> 8) & 0x1f),
             static_cast<uint32_t>(femtosecondsPerTick / 1000000),
             bits64 ? "64-bit" : "32-bit");

    maxValue = bits64 ? UINT64_MAX : UINT32_MAX;

    // Enable main counter and disable legacy replacement mapping
    writeRegister(GENERAL_CONFIGURATION, Configuration::ENABLE);
}

bool Hpet::isAvailable() {
    auto &acpi = Kernel::Service::getService<Kernel::InformationService>().getAcpi();
    return acpi.hasTable("HPET");
}

uint64_t Hpet::readRegister(uint16_t offset) {
    return *reinterpret_cast<uint64_t *>(baseAddress + offset);
}

void Hpet::writeRegister(uint16_t offset, uint64_t value) {
    *reinterpret_cast<uint64_t *>(baseAddress + offset) = value;
}

uint64_t Hpet::readCounter() {
    uint32_t counterLow, counterHigh, counterHighAfter;

    do {
        counterHigh = *reinterpret_cast<uint32_t *>(baseAddress + MAIN_COUNTER_VALUE + 4);
        counterLow = *reinterpret_cast<uint32_t *>(baseAddress + MAIN_COUNTER_VALUE);
        counterHighAfter = *reinterpret_cast<uint32_t *>(baseAddress + MAIN_COUNTER_VALUE + 4);
    } while (counterHigh != counterHighAfter);

    return counterLow | (static_cast<uint64_t>(counterHigh) << 32);
}

uint64_t Hpet::getFemtosecondsPerTick() const {
    return femtosecondsPerTick;
}

uint64_t Hpet::getMaxValue() const {
    return maxValue;
}

void Hpet::pluginSystemTimer() {
    // Setup first available timer as system timer
    auto &interruptService = Kernel::Service::getService<Kernel::InterruptService>();
    auto validInterrupts = Timer::getValidInterruptLines(*this, 0);
    InterruptRequest interrupt = validInterrupts[0];
    bool interruptFound = false;

    if (validInterrupts.contains(InterruptRequest::PIT)) {
        // Try to use the PIT interrupt, since the PIT is definitely not in use when the HPET is used
        interrupt = InterruptRequest::PIT;
        interruptFound = true;
    } else {
        // Timer cannot be used with the PIT interrupt -> Try to find an interrupt number above the classic PC/AT interrupts
        for (const auto &currentInterrupt: validInterrupts) {
            if (currentInterrupt > InterruptRequest::SECONDARY_ATA && currentInterrupt <= static_cast<uint32_t>(interruptService.getMaxInterruptTarget())) {
                interrupt = currentInterrupt;
                interruptFound = true;
                break;
            }
        }
    }

    if (!interruptFound) {
        // As a last resort, try to use one of the three free PC/AT interrupts
        if (validInterrupts.contains(InterruptRequest::FREE1)) {
            interrupt = InterruptRequest::FREE1;
        } else if (validInterrupts.contains(InterruptRequest::FREE2)) {
            interrupt = InterruptRequest::FREE2;
        } else if (validInterrupts.contains(InterruptRequest::FREE3)) {
            interrupt = InterruptRequest::FREE3;
        }
    }

    LOG_INFO("Using interrupt [%u] for system timer", interrupt);

    systemTimer = new Timer(*this, 0, interrupt);
    systemTimerInterruptHandler = new SystemTimerInterruptHandler(*this, *systemTimer);

    systemTimer->plugin();
    systemTimerInterruptHandler->armTimer();
}

void Hpet::wait(const Util::Time::Timestamp &waitTime) {
    Util::Time::Timestamp elapsedTime;
    auto lastTimerValue = readCounter();

    while (elapsedTime < waitTime) {
        auto timerValue = readCounter();
        auto ticks = timerValue >= lastTimerValue ? timerValue - lastTimerValue : maxValue - lastTimerValue + timerValue;

        elapsedTime += Util::Time::Timestamp::ofNanoseconds((ticks * femtosecondsPerTick) / 1000000);
        lastTimerValue = timerValue;
    }
}

Util::Time::Timestamp Hpet::getTime() {
    return systemTimerInterruptHandler->getTime();
}

}