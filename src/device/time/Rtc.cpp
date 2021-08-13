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
#include "Rtc.h"
#include "Cmos.h"

namespace Device {

Rtc::Rtc() {
    useBcd = !(Cmos::read(STATUS_REGISTER_B) & 0x04);
    useTwelveHours = !(Cmos::read(STATUS_REGISTER_B) & 0x02);
    currentDate = readDate();
}

Rtc &Rtc::getInstance() {
    static Rtc instance;
    return instance;
}

void Rtc::plugin() {
    Cpu::disableInterrupts();
    Cmos::disableNmi();

    // Enable 'Update interrupts': An Interrupt will be triggered after every RTC-update.
    uint8_t oldValue = Cmos::read(STATUS_REGISTER_B);
    Cmos::write(STATUS_REGISTER_B, oldValue | 0x10);

    // Set the periodic interrupt rate.
    oldValue = Cmos::read(STATUS_REGISTER_A);
    Cmos::write(STATUS_REGISTER_A, (oldValue & 0xF0u) | RTC_RATE);

    // Read Register C. This will clear the data-flag.
    // As long as this flag is set, the RTC won't trigger any interrupts.
    Cmos::read(STATUS_REGISTER_C);

    Kernel::InterruptDispatcher::getInstance().assign(40, *this);
    Pic::getInstance().allow(Pic::Interrupt::RTC);

    Cmos::enableNmi();
    Cpu::enableInterrupts();
}

void Rtc::trigger(Kernel::InterruptFrame &frame) {
    if((Cmos::read(STATUS_REGISTER_C) & 0x10) != 0x10) {
        return;
    }

    currentDate = readDate();
}

DateProvider::Date Rtc::getCurrentDate() {
    return currentDate;
}

void Rtc::setHardwareDate(const DateProvider::Date &date) {
    Date outDate = date;
    uint8_t century;

    if(outDate.year < 100) {
        century = CURRENT_CENTURY;
    } else {
        century = outDate.year / 100;
        outDate.year = outDate.year % 100;
    }

    if(useTwelveHours) {
        outDate.hours = outDate.hours % 12;
    }

    if(useBcd) {
        outDate.seconds = binaryToBcd(outDate.seconds);
        outDate.minutes = binaryToBcd(outDate.minutes);
        outDate.hours = binaryToBcd(outDate.hours);
        outDate.dayOfMonth = binaryToBcd(outDate.dayOfMonth);
        outDate.month = binaryToBcd(outDate.month);
        outDate.year = binaryToBcd(outDate.year);
        century = binaryToBcd(century);
    }

    while(isUpdating());

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
    while(isUpdating());

    Date date;
    date.seconds = Cmos::read(SECONDS_REGISTER);
    date.minutes = Cmos::read(MINUTES_REGISTER);
    date.hours = Cmos::read(HOURS_REGISTER);
    date.dayOfMonth = Cmos::read(DAY_OF_MONTH_REGISTER);
    date.month = Cmos::read(MONTH_REGISTER);
    date.year = Cmos::read(YEAR_REGISTER);

    uint8_t century = Cmos::read(CENTURY_REGISTER);
    if(century == 0) {
        century = CURRENT_CENTURY;
    }

    if(useBcd) {
        date.seconds = bcdToBinary(date.seconds);
        date.minutes = bcdToBinary(date.minutes);
        date.hours = bcdToBinary(date.hours) | (date.hours & 0x80);
        date.dayOfMonth = bcdToBinary(date.dayOfMonth);
        date.month = bcdToBinary(date.month);
        date.year = bcdToBinary(date.year);
        century = bcdToBinary(century);
    }

    date.year += century * 100;

    if(useTwelveHours && (date.hours & 0x80)) {
        date.hours = ((date.hours & 0x7F) + 12) % 24;
    }

    return date;
}

}