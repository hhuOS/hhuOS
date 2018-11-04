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

#ifndef __RTC__include__
#define __RTC__include__

#include <kernel/log/Logger.h>
#include "kernel/IOport.h"
#include "IODevice.h"

/**
 * Driver for the CMOS Realtime clock.
 *
 * @author Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schoettner
 * @date HHU, 2017
 */
class Rtc : public IODevice {

public:
    /**
     * Contains Date- and Time-information.
     */
    struct Date {
        uint8_t seconds = 0;
        uint8_t minutes = 0;
        uint8_t hours = 0;
        uint8_t dayOfMonth = 0;
        uint8_t month = 0;
        uint16_t year = 0;

        bool operator == (Date &date) {
            return seconds == date.seconds && minutes == date.minutes && hours == date.hours &&
                   dayOfMonth == date.dayOfMonth && month == date.month && year == date.year;
        }

        bool operator != (Date &date) {
            return seconds != date.seconds || minutes != date.minutes || hours != date.hours ||
                   dayOfMonth != date.dayOfMonth || month != date.month || year != date.year;
        }
    };

private:
    /**
     * RTC-Registers.
     */
    enum REGISTERS {
        SECONDS_REGISTER = 0x00,
        MINUTES_REGISTER = 0x02,
        HOURS_REGISTER = 0x04,
        DAY_OF_WEEK_REGISTER = 0x06,
        DAY_OF_MONTH_REGISTER = 0x07,
        MONTH_REGISTER = 0x08,
        YEAR_REGISTER = 0x09,
        CENTURY_REGISTER = 0x32,
        STATUS_REGISTER_A = 0x0A,
        STATUS_REGISTER_B = 0x0B,
        STATUS_REGISTER_C = 0x0C
    };

    IOport registerPort, dataPort;

    Rtc::Date currentDate {};

    bool useBcd;

    bool useTwelveHours;

    static Logger &log;

    static const uint8_t RTC_RATE = 0x06;

    static const uint8_t CURRENT_CENTURY = 0x20;

private:

    uint8_t bcdToBinary(uint8_t bcd);

    uint8_t binaryToBcd(uint8_t binary);

public:
    /**
     * Constructor.
     */
    Rtc();

    /**
     * Copy-constructor.
     */
    Rtc(const Rtc &copy) = delete;

    /**
     * Destructor.
     */
    ~Rtc() override = default;

    /**
     * Enable periodic interrupts for the RTC.
     */
    void plugin();

    /**
     * Overriding function from IODevice.
     */
    void trigger(InterruptFrame &frame) override;

    /**
     * Overriding function from IODevice.
     */
    bool checkForData() override;

    /**
     * Checks, if the RTC is currently updating.
     * Read/Write operations should not be performed, while an update is in progress.
     */
    bool isUpdating();

    /**
     * Get the current date.
     */
    Date getCurrentDate();

    /**
     * Set the RTC's date.
     */
    void setHardwareDate(const Date &date);
};

#endif
