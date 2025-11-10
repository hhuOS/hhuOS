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

#include "AcpiTimer.h"

#include "device/system/Acpi.h"
#include "kernel/service/Service.h"
#include "kernel/service/InformationService.h"
#include "kernel/log/Log.h"
#include "lib/util/base/Panic.h"
#include "lib/util/hardware/Acpi.h"
#include "lib/util/time/Timestamp.h"

Device::AcpiTimer::AcpiTimer() {
    if (!isAvailable()) {
        Util::Panic::fire(Util::Panic::UNSUPPORTED_OPERATION, "Trying to initialize unavailable ACPI timer!");
    }

    auto &acpi = Kernel::Service::getService<Kernel::InformationService>().getAcpi();
    const auto &fadt = reinterpret_cast<const Util::Hardware::Acpi::Fadt&>(acpi.getTables()["FACP"]);

    timerPort = IoPort(fadt.pmTimerBlock);

    if (fadt.flags & Util::Hardware::Acpi::EXTENDED_TIMER_VALUE) {
        LOG_INFO("ACPI timer has a 32-bit counter");
        maxValue = UINT32_MAX;
    } else {
        LOG_INFO("ACPI timer has a 24-bit counter");
        maxValue = 0x00ffffff;
    }
}

bool Device::AcpiTimer::isAvailable() {
    auto &acpiTables = Kernel::Service::getService<Kernel::InformationService>().getAcpi().getTables();
    if (!acpiTables.hasTable("FACP")) {
        return false;
    }

    const auto &fadt = reinterpret_cast<const Util::Hardware::Acpi::Fadt&>(acpiTables["FACP"]);
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
