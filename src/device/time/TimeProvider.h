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

#ifndef HHUOS_TIMEPROVIDER_H
#define HHUOS_TIMEPROVIDER_H

#include <cstdint>

namespace Device {

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

        [[nodiscard]] uint32_t toNanos() const;

        [[nodiscard]] uint32_t toMicros() const;

        [[nodiscard]] uint32_t toMillis() const;

        [[nodiscard]] uint32_t toSeconds() const;

        [[nodiscard]] uint32_t toMinutes() const;

        [[nodiscard]] uint32_t toHours() const;

        [[nodiscard]] uint32_t toDays() const;

        [[nodiscard]] uint32_t toYears() const;
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

    /**
     * Default Constructor.
     */
    TimeProvider() = default;

    /**
     * Copy constructor.
     */
    TimeProvider(const TimeProvider &other) = delete;

    /**
     * Assignment operator.
     */
    TimeProvider &operator=(const TimeProvider &other) = delete;

    /**
     * Destructor.
     */
    virtual ~TimeProvider() = default;

    [[nodiscard]] virtual Time getTime() = 0;

    static uint32_t convert(uint32_t value, TimeProvider::TimeUnit from, TimeProvider::TimeUnit to);

private:

    static uint32_t conversionMap[8][8];

};

}

#endif