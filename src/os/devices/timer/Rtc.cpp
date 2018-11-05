/*
 * Copyright (C) 2018 Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schoettner
 * Heinrich-Heine University
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

#include <kernel/interrupts/IntDispatcher.h>
#include <kernel/interrupts/Pic.h>
#include <kernel/services/TimeService.h>
#include <kernel/threads/Scheduler.h>
#include "devices/IODeviceManager.h"
#include "Rtc.h"


Logger &Rtc::log = Logger::get("RTC");

Rtc::Rtc() : registerPort(0x70), dataPort(0x71) {
    registerPort.outb(STATUS_REGISTER_B);
    useBcd = !(dataPort.inb() & 0x04);

    registerPort.outb(STATUS_REGISTER_B);
    useTwelveHours = !(dataPort.inb() & 0x02);
}

uint8_t Rtc::bcdToBinary(uint8_t bcd) {
    return (bcd & 0x0F) + ((bcd / 16) * 10);
}

uint8_t Rtc::binaryToBcd(uint8_t binary) {
    return (binary % 10) + ((binary / 10) << 4);
}

void Rtc::plugin() {
    Cpu::disableInterrupts();

    // Disable NMIs
    uint8_t oldValue = registerPort.inb();
    registerPort.outb(static_cast<uint8_t>(oldValue | 0x80u));

    log.trace("Disabled NMIs");

    // Enable 'Update interrupts': An Interrupt will be triggered after every RTC-update.
    registerPort.outb(STATUS_REGISTER_B);
    oldValue = dataPort.inb();

    registerPort.outb(STATUS_REGISTER_B);
    dataPort.outb(static_cast<uint8_t>(oldValue | 0x10u));

    // Set the periodic interrupt rate.
    registerPort.outb(STATUS_REGISTER_A);
    oldValue = dataPort.inb();

    registerPort.outb(STATUS_REGISTER_A);
    dataPort.outb(static_cast<uint8_t>((oldValue & 0xF0u) | RTC_RATE));

    // Read Register C. This will clear data-flag. As long as this flag is set,
    // the RTC won't trigger any interrupts.
    registerPort.outb(STATUS_REGISTER_C);
    dataPort.inb();

    log.trace("Setup RTC");

    IntDispatcher::getInstance().assign(40, *this);
    Pic::getInstance()->allow(Pic::Interrupt::RTC);

    log.trace("Registered RTC interrupt handler");

    // Enable NMIs
    oldValue = registerPort.inb();
    registerPort.outb(static_cast<uint8_t>(oldValue & 0x7Fu));

    log.trace("Enabled NMIs");

    Cpu::enableInterrupts();

    IODeviceManager::getInstance().registerIODevice(this);
}

void Rtc::trigger(InterruptFrame &frame) {
    registerPort.outb(STATUS_REGISTER_C);
    dataPort.inb();

    registerPort.outb(SECONDS_REGISTER);
    currentDate.seconds = dataPort.inb();

    registerPort.outb(MINUTES_REGISTER);
    currentDate.minutes = dataPort.inb();

    registerPort.outb(HOURS_REGISTER);
    currentDate.hours = dataPort.inb();

    registerPort.outb(DAY_OF_MONTH_REGISTER);
    currentDate.dayOfMonth = dataPort.inb();

    registerPort.outb(MONTH_REGISTER);
    currentDate.month = dataPort.inb();

    registerPort.outb(YEAR_REGISTER);
    currentDate.year = dataPort.inb();

    registerPort.outb(CENTURY_REGISTER);
    uint8_t century = dataPort.inb();

    if(century == 0) {
        century = CURRENT_CENTURY;
    }

    if(useBcd) {
        currentDate.seconds = bcdToBinary(currentDate.seconds);
        currentDate.minutes = bcdToBinary(currentDate.minutes);
        currentDate.hours = bcdToBinary(currentDate.hours) | (currentDate.hours & 0x80);
        currentDate.dayOfMonth = bcdToBinary(currentDate.dayOfMonth);
        currentDate.month = bcdToBinary(currentDate.month);
        currentDate.year = bcdToBinary(currentDate.year);
        century = bcdToBinary(century);
    }

    currentDate.year += century * 100;

    if(useTwelveHours && (currentDate.hours & 0x80)) {
        currentDate.hours = static_cast<uint8_t>(((currentDate.hours & 0x7F) + 12) % 24);
    }
}

bool Rtc::checkForData() {
    registerPort.outb(STATUS_REGISTER_C);
    return (dataPort.inb() & 0x10) == 0x10;
}

bool Rtc::isUpdating() {
    registerPort.outb(STATUS_REGISTER_A);

    return dataPort.inb() & 0x80;
}

Rtc::Date Rtc::getCurrentDate() {
    return Rtc::currentDate;
}

void Rtc::setHardwareDate(const Date &date) {
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

    registerPort.outb(SECONDS_REGISTER);
    dataPort.outb(outDate.seconds);

    registerPort.outb(MINUTES_REGISTER);
    dataPort.outb(outDate.minutes);

    registerPort.outb(HOURS_REGISTER);
    dataPort.outb(outDate.hours);

    registerPort.outb(DAY_OF_MONTH_REGISTER);
    dataPort.outb(outDate.dayOfMonth);

    registerPort.outb(MONTH_REGISTER);
    dataPort.outb(outDate.month);

    registerPort.outb(YEAR_REGISTER);
    dataPort.outb(outDate.year);

    registerPort.outb(CENTURY_REGISTER);
    dataPort.outb(century);
}
