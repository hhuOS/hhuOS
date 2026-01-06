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

#include "device/cpu/Cpu.h"
#include "kernel/service/InterruptService.h"
#include "Cmos.h"
#include "Rtc.h"
#include "kernel/service/ProcessService.h"
#include "device/interrupt/InterruptRequest.h"
#include "device/system/Acpi.h"
#include "AlarmRunnable.h"
#include "kernel/interrupt/InterruptVector.h"
#include "kernel/log/Log.h"
#include "kernel/process/Thread.h"
#include "lib/util/hardware/Acpi.h"
#include "kernel/service/InformationService.h"
#include "kernel/service/Service.h"
#include "kernel/process/Scheduler.h"

namespace Kernel {
struct InterruptFrame;
}  // namespace Kernel

namespace Device {

Rtc::Rtc(const Util::Time::Timestamp &timerInterval) {
    const auto &acpiTables = Kernel::Service::getService<Kernel::InformationService>().getAcpi().getTables();
    if (acpiTables.hasTable("FADT")) {
        const auto &fadt = reinterpret_cast<const Util::Hardware::Acpi::Fadt&>(acpiTables["FADT"]);
        centuryRegister = fadt.century;
    }

    useBcd = (Cmos::read(STATUS_REGISTER_B) & 0x04) == 0;
    useTwelveHours = (Cmos::read(STATUS_REGISTER_B) & 0x02) == 0;
    currentDate = readDate();
    LOG_INFO("Retrieved RTC state (Data format: [%s], Hour format: [%s], Century register: [0x%02x], Current date: [%u-%02u-%02u %02u:%02u:%02u]",
             useBcd ? "BCD" : "Binary", useTwelveHours ? "12h" : "24h", centuryRegister,
             currentDate.getYear(), currentDate.getMonth(), currentDate.getDayOfMonth(),
             currentDate.getHours(), currentDate.getMinutes(), currentDate.getSeconds());

    setInterruptRate(timerInterval);
}

void Rtc::plugin() {
    Cpu::disableInterrupts();
    Cmos::disableNmi();

    auto &interruptService = Kernel::Service::getService<Kernel::InterruptService>();
    interruptService.assignInterrupt(Kernel::InterruptVector::RTC, *this);
    interruptService.allowHardwareInterrupt(Device::InterruptRequest::RTC);

    Cmos::enableNmi();
    Cpu::enableInterrupts();
}

void Rtc::trigger([[maybe_unused]] const Kernel::InterruptFrame &frame, [[maybe_unused]] Kernel::InterruptVector slot) {
    uint8_t interruptStatus = Cmos::read(STATUS_REGISTER_C);
    if ((interruptStatus & INTERRUPT_UPDATE_ENDED) != 0) {
        currentDate = readDate();
    }

    if ((interruptStatus & INTERRUPT_ALARM) != 0) {
        alarm();
    }

    if ((interruptStatus & INTERRUPT_PERIODIC) != 0) {
        time += timerInterval;
    }
}

bool Rtc::isValid() {
    // The high bit in status register D indicates validity
    uint8_t value = Cmos::read(STATUS_REGISTER_D);
    return (value & 0x80) != 0;
}

Util::Time::Date Rtc::getCurrentDate() {
    return currentDate;
}

void Rtc::setCurrentDate(const Util::Time::Date &date) {
    auto seconds = date.getSeconds();
    auto minutes = date.getMinutes();
    auto hours = date.getHours();
    auto dayOfMonth = date.getDayOfMonth();
    auto month = date.getMonth();
    auto year = date.getYear();
    uint8_t century;

    if (year < 100) {
        century = CURRENT_CENTURY;
    } else {
        century = year / 100;
        year %= 100;
    }

    if (useTwelveHours) {
        hours = hours % 12;
    }

    if (useBcd) {
        seconds = binaryToBcd(seconds);
        minutes = binaryToBcd(minutes);
        hours = binaryToBcd(hours);
        dayOfMonth = binaryToBcd(dayOfMonth);
        month = binaryToBcd(month);
        year = binaryToBcd(year);
        century = binaryToBcd(century);
    }

    while (isUpdating()) {}

    Cpu::disableInterrupts();
    Cmos::disableNmi();

    Cmos::write(SECONDS_REGISTER, seconds);
    Cmos::write(MINUTES_REGISTER, minutes);
    Cmos::write(HOURS_REGISTER, hours);
    Cmos::write(DAY_OF_MONTH_REGISTER, dayOfMonth);
    Cmos::write(MONTH_REGISTER, month);
    Cmos::write(YEAR_REGISTER, year);

    if (centuryRegister != 0) {
        Cmos::write(centuryRegister, century);
    }

    currentDate = date;

    Cmos::enableNmi();
    Cpu::enableInterrupts();
}

void Rtc::setAlarm(const Util::Time::Date &date) const {
    auto seconds = date.getSeconds();
    auto minutes = date.getMinutes();
    auto hours = date.getHours();

    if (useTwelveHours) {
        hours = date.getHours() % 12;
    }

    if (useBcd) {
        seconds = binaryToBcd(seconds);
        minutes = binaryToBcd(minutes);
        hours = binaryToBcd(hours);
    }

    Cmos::write(ALARM_SECONDS_REGISTER, seconds);
    Cmos::write(ALARM_MINUTES_REGISTER, minutes);
    Cmos::write(ALARM_HOURS_REGISTER, hours);

    // Enable 'alarm interrupts': An Interrupt will be triggered every 24 hours at the set alarm time.
    uint8_t oldValue = Cmos::read(STATUS_REGISTER_B);
    Cmos::write(STATUS_REGISTER_B, oldValue | INTERRUPT_ALARM);
}

uint8_t Rtc::bcdToBinary(uint8_t bcd) {
    return (bcd & 0x0F) + ((bcd / 16) * 10);
}

uint8_t Rtc::binaryToBcd(uint8_t binary) {
    return (binary % 10) + ((binary / 10) << 4);
}

bool Rtc::isUpdating() {
    return (Cmos::read(STATUS_REGISTER_A) & 0x80) == 0x80;
}

Util::Time::Date Rtc::readDate() const {
    while (isUpdating()) {}

    auto seconds = Cmos::read(SECONDS_REGISTER);
    auto minutes = Cmos::read(MINUTES_REGISTER);
    auto hours = Cmos::read(HOURS_REGISTER);
    auto dayOfMonth = Cmos::read(DAY_OF_MONTH_REGISTER);
    auto month = Cmos::read(MONTH_REGISTER);
    auto year = Cmos::read(YEAR_REGISTER);
    uint8_t century;

    if (centuryRegister != 0) {
        century = Cmos::read(centuryRegister);
    } else {
        century = useBcd ? binaryToBcd(CURRENT_CENTURY) : CURRENT_CENTURY;
    }

    if (useBcd) {
        seconds = bcdToBinary(seconds);
        minutes = bcdToBinary(minutes);
        hours = bcdToBinary(hours | (hours & 0x80));
        dayOfMonth = bcdToBinary(dayOfMonth);
        month = bcdToBinary(month);
        year = bcdToBinary(year);
        century = bcdToBinary(century);
    }

    if (useTwelveHours && (hours & 0x80)) {
        hours = ((hours & 0x7F) + 12) % 24;
    }

    return Util::Time::Date(seconds, minutes, hours, dayOfMonth, month, static_cast<int16_t>((century * 100) + year));
}

void Rtc::alarm() {
    auto &alarmThread = Kernel::Thread::createKernelThread("Rtc-Alarm", Kernel::Service::getService<Kernel::ProcessService>().getKernelProcess(), new AlarmRunnable());
    Kernel::Service::getService<Kernel::ProcessService>().getScheduler().ready(alarmThread);
}

void Rtc::setInterruptRate(const Util::Time::Timestamp &interval) {
    Cpu::disableInterrupts();
    Cmos::disableNmi();

    if (interval.toNanoseconds() == 0) {
        // Periodic interrupts are disabled
        timerInterval = Util::Time::Timestamp::ofMilliseconds(0);
    } else {
        auto divisor = interval.toNanoseconds() / NANOSECONDS_PER_TICK;
        uint32_t divisorLog;
        for (divisorLog = 1; divisor >= 2; divisorLog++) {
            divisor /= 2;
        }

        if (divisorLog < 3) divisorLog = 3;
        if (divisorLog > 15) divisorLog = 15;

        timerInterval = Util::Time::Timestamp::ofNanoseconds(static_cast<uint32_t>(NANOSECONDS_PER_TICK * (1 << (divisorLog - 1))));
        LOG_INFO("Setting RTC interval to [%ums] (Divisor: [%u])", timerInterval.toMilliseconds() < 1 ? 1 : timerInterval.toMilliseconds(), divisorLog);

        // Set periodic interrupt rate
        auto oldValue = Cmos::read(STATUS_REGISTER_A);
        Cmos::write(STATUS_REGISTER_A, (oldValue & 0xf0) | divisorLog);
    }

    // Enable 'update ended interrupts': An Interrupt will be triggered after every RTC-update.
    // Enable 'periodic interrupts': Periodic interrupts will be triggered at the set rate.
    auto oldValue = Cmos::read(STATUS_REGISTER_B);
    Cmos::write(STATUS_REGISTER_B, oldValue | INTERRUPT_UPDATE_ENDED | (timerInterval.toNanoseconds() == 0 ? 0 : INTERRUPT_PERIODIC));

    // Read status register C. This will clear the data-flag.
    // As long as this flag is set, the RTC won't trigger any interrupts.
    Cmos::read(STATUS_REGISTER_C);

    Cmos::enableNmi();
    Cpu::enableInterrupts();
}

Util::Time::Timestamp Rtc::getTime() {
    return time;
}

bool Rtc::isAvailable() {
    Cpu::disableInterrupts();
    Cmos::disableNmi();

    uint8_t oldValue = Cmos::read(STATUS_REGISTER_A);

    for (uint8_t i = 3; i <= 15; i++) {
        Cmos::write(STATUS_REGISTER_A, (oldValue & 0xf0) | i);
        uint8_t currentValue = Cmos::read(STATUS_REGISTER_A) & 0x0f;

        if (currentValue != i) {
            Cmos::enableNmi();
            Cpu::enableInterrupts();
            return false;
        }
    }

    Cmos::write(STATUS_REGISTER_A, oldValue);

    Cmos::enableNmi();
    Cpu::enableInterrupts();
    return true;
}

}