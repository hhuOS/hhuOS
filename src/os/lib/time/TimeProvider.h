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

#ifndef __TimeProvider_include__
#define __TimeProvider_include__

#include <cstdint>

class TimeProvider {

public:

    struct Time {
        uint32_t seconds = 0;
        uint32_t fraction = 0;

        void addNanos(uint32_t value);

        void addSeconds(uint32_t value);

        bool operator>(const Time &other) const;

        bool operator>=(const Time &other) const;

        bool operator<(const Time &other) const;

        bool operator<=(const Time &other) const;

        uint32_t toNanos();

        uint32_t toMicros();

        uint32_t toMillis();

        uint32_t toSeconds();

        uint32_t toMinutes();

        uint32_t toHours();

        uint32_t toDays();

        uint32_t toYears();
    };

    enum TimeUnit {
        NANO    = 0x01,
        MICRO   = 0x02,
        MILLI   = 0x03,
        SECONDS = 0x04,
        MINUTES = 0x05,
        HOURS   = 0x06,
        DAYS    = 0x07,
        YEARS   = 0x08
    };

    static uint32_t conversionMap[8][8];

    TimeProvider() noexcept = default;

    virtual ~TimeProvider() = default;

    TimeProvider(const TimeProvider &other) = delete;

    TimeProvider &operator=(const TimeProvider &other) = delete;

    virtual uint32_t getNanos() = 0;

    virtual uint32_t getMicros() = 0;

    virtual uint32_t getMillis() = 0;

    virtual uint32_t getSeconds() = 0;

    virtual uint32_t getMinutes() = 0;

    virtual uint32_t getHours() = 0;

    virtual uint32_t getDays() = 0;

    virtual uint32_t getYears() = 0;

    Time getTime() const {

        return time;
    }

    uint32_t getTicks() const {

        return ticks;
    };

    static uint32_t convert(uint32_t value, TimeProvider::TimeUnit from, TimeProvider::TimeUnit to);

protected:

    uint32_t ticks = 0;

    Time time;
};

#endif
