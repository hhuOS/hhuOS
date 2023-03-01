/*
 * Copyright (C) 2018-2023 Heinrich-Heine-Universitaet Duesseldorf,
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

#ifndef HHUOS_TIMESTAMP_H
#define HHUOS_TIMESTAMP_H

#include <cstdint>

namespace Util::Time {

class Timestamp {

public:

    enum TimeUnit {
        NANO = 0x01,
        MICRO = 0x02,
        MILLI = 0x03,
        SECONDS = 0x04,
        MINUTES = 0x05,
        HOURS = 0x06,
        DAYS = 0x07,
        YEARS = 0x08
    };

    /**
     * Default Constructor.
     */
    Timestamp() = default;

    Timestamp(uint32_t seconds, uint32_t fraction);

    /**
     * Copy Constructor.
     */
    Timestamp(const Timestamp &copy) = default;

    /**
     * Assignment operator.
     */
    Timestamp &operator=(const Timestamp &other) = default;

    /**
     * Destructor.
     */
    ~Timestamp() = default;

    [[nodiscard]] static uint32_t convert(uint32_t value, TimeUnit from, TimeUnit to);

    static Timestamp ofMilliseconds(uint32_t milliseconds);

    void addNanoseconds(uint32_t value);

    void addSeconds(uint32_t value);

    bool operator>(const Timestamp &other) const;

    bool operator>=(const Timestamp &other) const;

    bool operator<(const Timestamp &other) const;

    bool operator<=(const Timestamp &other) const;

    [[nodiscard]] uint32_t toNanoseconds() const;

    [[nodiscard]] uint32_t toMicroseconds() const;

    [[nodiscard]] uint32_t toMilliseconds() const;

    [[nodiscard]] uint32_t toSeconds() const;

    [[nodiscard]] uint32_t toMinutes() const;

    [[nodiscard]] uint32_t toHours() const;

    [[nodiscard]] uint32_t toDays() const;

    [[nodiscard]] uint32_t toYears() const;

private:

    uint32_t seconds = 0;
    uint32_t fraction = 0;

    static const constexpr uint32_t conversionMap[8][8] = {
            {1,          1000,       1000000,  1000000000, 1,        1,          1,        1},
            {1000,       1,          1000,     1000000,    60000000, 3600000000, 1,        1},
            {1000000,    1000,       1,        1000,       60000,    3600000,    86400000, 1},
            {1000000000, 1000000,    1000,     1,          60,       3600,       864000,   31536000},
            {1,          60000000,   60000,    60,         1,        60,         1440,     525600},
            {1,          3600000000, 3600000,  3600,       60,       1,          24,       8760},
            {1,          1,          86400000, 864000,     1440,     24,         1,        365},
            {1,          1,          1,        31536000,   525600,   8760,       365,      1}
    };

    // Works only on 64-bit systems
    //static const constexpr uint64_t conversionMap[8][8] = {
    //        {                1,           1000,     1000000, 1000000000, 60000000000, 3600000000000, 86400000000000, 31536000000000000},
    //        {             1000,              1,        1000,    1000000,    60000000,    3600000000,    86400000000,    31536000000000},
    //        {          1000000,           1000,           1,       1000,       60000,       3600000,       86400000,       31536000000},
    //        {       1000000000,        1000000,        1000,          1,          60,          3600,         864000,          31536000},
    //        {      60000000000,       60000000,       60000,         60,           1,            60,           1440,            525600},
    //        {    3600000000000,     3600000000,     3600000,       3600,          60,             1,             24,              8760},
    //        {   86400000000000,    86400000000,    86400000,     864000,        1440,            24,              1,               365},
    //        {31536000000000000, 31536000000000, 31536000000,   31536000,      525600,          8760,            365,                 1}
    //};

};

Timestamp getSystemTime();

}

#endif