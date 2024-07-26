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

#include "AcpiTimer.h"

#include "device/system/Acpi.h"
#include "kernel/service/Service.h"
#include "kernel/service/InformationService.h"
#include "kernel/log/Log.h"
#include "lib/util/base/Exception.h"
#include "lib/util/hardware/Acpi.h"
#include "lib/util/time/Timestamp.h"

Device::AcpiTimer::AcpiTimer() {
    if (!isAvailable()) {
        Util::Exception::throwException(Util::Exception::UNSUPPORTED_OPERATION, "Trying to initialize unavailable ACPI timer!");
    }

    auto &acpi = Kernel::Service::getService<Kernel::InformationService>().getAcpi();
    const auto &fadt = acpi.getTable<Util::Hardware::Acpi::Fadt>("FACP");

    timerPort = IoPort(fadt.pmTimerBlock);

    if (fadt.flags & 1 << 8) {
        LOG_INFO("ACPI timer has a 32-bit counter");
        maxValue = UINT32_MAX;
    } else {
        LOG_INFO("ACPI timer has a 24-bit counter");
        maxValue = 0x00ffffff;
    }
}

bool Device::AcpiTimer::isAvailable() {
    auto &acpi = Kernel::Service::getService<Kernel::InformationService>().getAcpi();
    if (!acpi.hasTable("FACP")) {
        return false;
    }

    const auto &fadt = acpi.getTable<Util::Hardware::Acpi::Fadt>("FACP");
    return fadt.pmTimerLength == 4; // See https://wiki.osdev.org/ACPI_Timer
}

uint32_t Device::AcpiTimer::readTimer() {
    return timerPort.readDoubleWord();
}

void Device::AcpiTimer::wait(const Util::Time::Timestamp &waitTime) {
    Util::Time::Timestamp elapsedTime;
    auto lastTimerValue = readTimer();

    while (elapsedTime < waitTime) {
        auto timerValue = readTimer();
        auto ticks = timerValue >= lastTimerValue ? timerValue - lastTimerValue : maxValue - lastTimerValue + timerValue;

        elapsedTime += Util::Time::Timestamp::ofNanoseconds(ticks * NANOSECONDS_PER_TICK);
        lastTimerValue = timerValue;
    }
}
