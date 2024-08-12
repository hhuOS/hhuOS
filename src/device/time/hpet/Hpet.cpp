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
 *
 * The HPET driver is based on a bachelor's thesis, written by Suratsch Hassan.
 * The original source code can be found here: https://git.hhu.de/bsinfo/thesis/ba-suhas102
 */

#include "Hpet.h"
#include "device/system/Acpi.h"
#include "kernel/service/InformationService.h"
#include "kernel/log/Log.h"
#include "kernel/service/MemoryService.h"

namespace Device {

Hpet::Hpet() : baseAddress() {
    auto &acpi = Kernel::Service::getService<Kernel::InformationService>().getAcpi();
    if (!acpi.hasTable("HPET")) {
        Util::Exception::throwException(Util::Exception::UNSUPPORTED_OPERATION, "Trying to initialize non-existent HPET!");
    }

    const auto &hpetTable = acpi.getTable<Util::Hardware::Acpi::Hpet>("HPET");
    LOG_INFO("Found HPET (Address: [0x%08x])", hpetTable.address.address);

    auto &memoryService = Kernel::Service::getService<Kernel::MemoryService>();
    baseAddress = static_cast<uint8_t*>(memoryService.mapIO(reinterpret_cast<uint8_t*>(hpetTable.address.address), 1));

    auto capabilities = readRegister(GENERAL_CAPABILITIES_ID);
    auto timerCount = static_cast<uint8_t>((capabilities >> 8) & 0x1f);
    bool bits64 = capabilities & (1 << 13);
    counterClockPeriod = static_cast<uint32_t>((capabilities >> 32) & 0xffffffff);
    LOG_INFO("Vendor: [0x%04x], Revision: [%u], Comparators: [%u], Counter clock period: [%u fs], Counter size: [%s]",
             static_cast<uint32_t>((capabilities >> 16) & 0xffff), static_cast<uint32_t>(capabilities & 0xff),
             timerCount, counterClockPeriod, bits64 ? "64-bit" : "32-bit");

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

void Hpet::wait(const Util::Time::Timestamp &waitTime) {
    Util::Time::Timestamp elapsedTime;
    auto lastTimerValue = readCounter();

    while (elapsedTime < waitTime) {
        auto timerValue = readCounter();
        auto ticks = timerValue >= lastTimerValue ? timerValue - lastTimerValue : maxValue - lastTimerValue + timerValue;

        elapsedTime += Util::Time::Timestamp::ofNanoseconds((ticks * counterClockPeriod) / 1000000);
        lastTimerValue = timerValue;
    }
}

}