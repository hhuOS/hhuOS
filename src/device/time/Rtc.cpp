/*
 * Copyright (C) 2018-2021 Heinrich-Heine-Universitaet Duesseldorf,
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

#include <device/cpu/Cpu.h>
#include <kernel/interrupt/InterruptDispatcher.h>
#include <device/interrupt/Pic.h>
#include <device/sound/PcSpeaker.h>
#include "Rtc.h"
#include "Cmos.h"

namespace Device {

Rtc::Rtc(uint8_t interruptRateDivisor) {
    useBcd = (Cmos::read(STATUS_REGISTER_B) & 0x04) == 0;
    useTwelveHours = (Cmos::read(STATUS_REGISTER_B) & 0x02) == 0;
    currentDate = readDate();
    setInterruptRate(interruptRateDivisor);
}

Rtc &Rtc::getInstance() {
    static Rtc instance;
    return instance;
}

void Rtc::plugin() {
    Cpu::disableInterrupts();
    Cmos::disableNmi();

    // Enable 'update ended interrupts': An Interrupt will be triggered after every RTC-update.
    // Enable 'periodic interrupts': Periodic interrupts will be triggered at the set rate.
    uint8_t oldValue = Cmos::read(STATUS_REGISTER_B);
    Cmos::write(STATUS_REGISTER_B, oldValue | INTERRUPT_UPDATE_ENDED | INTERRUPT_PERIODIC);

    // Read status register C. This will clear the data-flag.
    // As long as this flag is set, the RTC won't trigger any interrupts.
    Cmos::read(STATUS_REGISTER_C);

    Kernel::InterruptDispatcher::getInstance().assign(40, *this);
    Pic::getInstance().allow(Pic::Interrupt::RTC);

    Cmos::enableNmi();
    Cpu::enableInterrupts();
}

void Rtc::trigger(Kernel::InterruptFrame &frame) {
    uint8_t interruptStatus = Cmos::read(STATUS_REGISTER_C);

    if ((interruptStatus & INTERRUPT_UPDATE_ENDED) != 0) {
        currentDate = readDate();
    }

    if ((interruptStatus & INTERRUPT_ALARM) != 0) {
        alarm();
    }

    if ((interruptStatus & INTERRUPT_PERIODIC) != 0) {
        time.addNanos(timerInterval);
        advanceTime(timerInterval);
    }
}

bool Rtc::isValid() {
    // The high bit in status register D indicates validity
    uint8_t value = Cmos::read(STATUS_REGISTER_D);
    return (value & 0x80) != 0;
}

DateProvider::Date Rtc::getCurrentDate() {
    return currentDate;
}

void Rtc::setHardwareDate(const DateProvider::Date &date) {
    Date outDate = date;
    uint8_t century;

    if (outDate.year < 100) {
        century = CURRENT_CENTURY;
    } else {
        century = outDate.year / 100;
        outDate.year = outDate.year % 100;
    }

    if (useTwelveHours) {
        outDate.hours = outDate.hours % 12;
    }

    if (useBcd) {
        outDate.seconds = binaryToBcd(outDate.seconds);
        outDate.minutes = binaryToBcd(outDate.minutes);
        outDate.hours = binaryToBcd(outDate.hours);
        outDate.dayOfMonth = binaryToBcd(outDate.dayOfMonth);
        outDate.month = binaryToBcd(outDate.month);
        outDate.year = binaryToBcd(outDate.year);
        century = binaryToBcd(century);
    }

    while (isUpdating());

    Cpu::disableInterrupts();
    Cmos::disableNmi();

    Cmos::write(SECONDS_REGISTER, outDate.seconds);
    Cmos::write(MINUTES_REGISTER, outDate.minutes);
    Cmos::write(HOURS_REGISTER, outDate.hours);
    Cmos::write(DAY_OF_MONTH_REGISTER, outDate.dayOfMonth);
    Cmos::write(MONTH_REGISTER, outDate.month);
    Cmos::write(YEAR_REGISTER, outDate.year);
    Cmos::write(CENTURY_REGISTER, century);

    currentDate = date;

    Cmos::enableNmi();
    Cpu::enableInterrupts();
}

void Rtc::setAlarm(const DateProvider::Date &date) const {
    Date alarmDate = date;

    if (useTwelveHours) {
        alarmDate.hours = alarmDate.hours % 12;
    }

    if (useBcd) {
        alarmDate.seconds = binaryToBcd(alarmDate.seconds);
        alarmDate.minutes = binaryToBcd(alarmDate.minutes);
        alarmDate.hours = binaryToBcd(alarmDate.hours);
    }

    Cmos::write(ALARM_SECONDS_REGISTER, alarmDate.seconds);
    Cmos::write(ALARM_MINUTES_REGISTER, alarmDate.minutes);
    Cmos::write(ALARM_HOURS_REGISTER, alarmDate.hours);

    // Enable 'alarm interrupts': An Interrupt will be triggered every 24 hours at the set alarm time.
    uint8_t oldValue = Cmos::read(STATUS_REGISTER_B);
    Cmos::write(STATUS_REGISTER_B, oldValue | 0x20);
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

DateProvider::Date Rtc::readDate() const {
    while (isUpdating());

    Date date;
    date.seconds = Cmos::read(SECONDS_REGISTER);
    date.minutes = Cmos::read(MINUTES_REGISTER);
    date.hours = Cmos::read(HOURS_REGISTER);
    date.dayOfMonth = Cmos::read(DAY_OF_MONTH_REGISTER);
    date.month = Cmos::read(MONTH_REGISTER);
    date.year = Cmos::read(YEAR_REGISTER);

    uint8_t century = Cmos::read(CENTURY_REGISTER);
    if (century == 0) {
        century = CURRENT_CENTURY;
    }

    if (useBcd) {
        date.seconds = bcdToBinary(date.seconds);
        date.minutes = bcdToBinary(date.minutes);
        date.hours = bcdToBinary(date.hours) | (date.hours & 0x80);
        date.dayOfMonth = bcdToBinary(date.dayOfMonth);
        date.month = bcdToBinary(date.month);
        date.year = bcdToBinary(date.year);
        century = bcdToBinary(century);
    }

    date.year += century * 100;

    if (useTwelveHours && (date.hours & 0x80)) {
        date.hours = ((date.hours & 0x7F) + 12) % 24;
    }

    return date;
}

void Rtc::alarm() {
    static AlarmRunnable alarmRunnable;
    registerJob(alarmRunnable, 500000000, 6);
}

void Rtc::setInterruptRate(uint8_t divisor) {
    if (divisor < 3 || divisor > 15) {
        Util::Exception::throwException(Util::Exception::INVALID_ARGUMENT, "RTC: Interrupt rate divisor must be between 3 and 15");
    }

    uint8_t oldValue = Cmos::read(STATUS_REGISTER_A);
    Cmos::write(STATUS_REGISTER_A, (oldValue & 0xf0) | divisor);

    // Frequency is divided by 2^divisor
    timerInterval = static_cast<uint32_t>(1000000000 / (BASE_FREQUENCY / (1 << (divisor - 1))));
}

TimeProvider::Time Rtc::getTime() {
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