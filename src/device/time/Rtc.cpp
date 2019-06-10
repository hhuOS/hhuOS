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

#include "device/misc/Pic.h"
#include "kernel/service/TimeService.h"
#include "kernel/thread/Scheduler.h"
#include "kernel/interrupt/InterruptDispatcher.h"
#include "device/misc/Cmos.h"
#include "Rtc.h"

Logger &Rtc::log = Logger::get("RTC");

Rtc::Rtc() : interruptDataBuffer(1024) {
    useBcd = !(Cmos::readRegister(STATUS_REGISTER_B) & 0x04);
    useTwelveHours = !(Cmos::readRegister(STATUS_REGISTER_B) & 0x02);
}

uint8_t Rtc::bcdToBinary(uint8_t bcd) {
    return static_cast<uint8_t>((bcd & 0x0F) + ((bcd / 16) * 10));
}

uint8_t Rtc::binaryToBcd(uint8_t binary) {
    return static_cast<uint8_t>((binary % 10) + ((binary / 10) << 4));
}

void Rtc::plugin() {
    log.trace("Initializing RTC");
    
    if(!Cmos::isAvailable()) {
        log.error("No CMOS available! Aborting RTC setup...");
        
        return;
    }

    Cpu::disableInterrupts();
    Cmos::disableNmi();

    // Enable 'Update interrupts': An Interrupt will be triggered after every RTC-update.
    uint8_t oldValue = Cmos::readRegister(STATUS_REGISTER_B);
    Cmos::writeRegister(STATUS_REGISTER_B, static_cast<uint8_t>(oldValue | 0x10u));

    // Set the periodic interrupt rate.
    oldValue = Cmos::readRegister(STATUS_REGISTER_A);
    Cmos::writeRegister(STATUS_REGISTER_A, static_cast<uint8_t>((oldValue & 0xF0u) | RTC_RATE));

    // Read Register C. This will clear the data-flag.
    // As long as this flag is set, the RTC won't trigger any interrupts.
    Cmos::readRegister(STATUS_REGISTER_C);

    InterruptManager::getInstance().registerInterruptHandler(this);
    InterruptDispatcher::getInstance().assign(40, *this);
    Pic::getInstance().allow(Pic::Interrupt::RTC);

    Cmos::enableNmi();
    Cpu::enableInterrupts();

    log.trace("Finished initializing RTC");
}

void Rtc::trigger(InterruptFrame &frame) {
    if((Cmos::readRegister(STATUS_REGISTER_C) & 0x10) != 0x10) {
        return;
    }

    InterruptData data;

    data.seconds = Cmos::readRegister(SECONDS_REGISTER);
    data.minutes = Cmos::readRegister(MINUTES_REGISTER);
    data.hours = Cmos::readRegister(HOURS_REGISTER);
    data.dayOfMonth = Cmos::readRegister(DAY_OF_MONTH_REGISTER);
    data.month = Cmos::readRegister(MONTH_REGISTER);
    data.year = Cmos::readRegister(YEAR_REGISTER);
    data.century = Cmos::readRegister(CENTURY_REGISTER);

    interruptDataBuffer.push(data);
}

bool Rtc::hasInterruptData() {
    return !interruptDataBuffer.isEmpty();
}

void Rtc::parseInterruptData() {
    const InterruptData &data = interruptDataBuffer.pop();

    lock.acquire();

    currentDate.seconds = data.seconds;
    currentDate.minutes = data.minutes;
    currentDate.hours = data.hours;
    currentDate.dayOfMonth = data.dayOfMonth;
    currentDate.month = data.month;
    currentDate.year = data.year;

    uint8_t century = data.century;

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

    lock.release();
}

bool Rtc::isUpdating() {
    return (Cmos::readRegister(STATUS_REGISTER_A) & 0x80) == 0x80;
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

    lock.acquire();

    while(isUpdating());

    Cpu::disableInterrupts();
    Cmos::disableNmi();

    Cmos::writeRegister(SECONDS_REGISTER, outDate.seconds);
    Cmos::writeRegister(MINUTES_REGISTER, outDate.minutes);
    Cmos::writeRegister(HOURS_REGISTER, outDate.hours);
    Cmos::writeRegister(DAY_OF_MONTH_REGISTER, outDate.dayOfMonth);
    Cmos::writeRegister(MONTH_REGISTER, outDate.month);
    Cmos::writeRegister(YEAR_REGISTER, outDate.year);
    Cmos::writeRegister(CENTURY_REGISTER, century);

    Cmos::enableNmi();
    Cpu::enableInterrupts();

    lock.release();
}
