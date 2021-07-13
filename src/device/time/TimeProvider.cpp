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

#include "TimeProvider.h"

namespace Device {

uint32_t TimeProvider::conversionMap[8][8] = {
        {                1,           1000,     1000000, 1000000000, 1, 1, 1, 1},
        {             1000,              1,        1000,    1000000,    60000000,    3600000000,    1,    1},
        {          1000000,           1000,           1,       1000,       60000,       3600000,       86400000,       1},
        {       1000000000,        1000000,        1000,          1,          60,          3600,         864000,          31536000},
        {      1,       60000000,       60000,         60,           1,            60,           1440,            525600},
        {    1,     3600000000,     3600000,       3600,          60,             1,             24,              8760},
        {   1,    1,    86400000,     864000,        1440,            24,              1,               365},
        {1, 1, 1,   31536000,      525600,          8760,            365,                 1}
};

// Works only on 64-bit systems
//uint32_t TimeProvider::conversionMap[8][8] = {
//        {                1,           1000,     1000000, 1000000000, 60000000000, 3600000000000, 86400000000000, 31536000000000000},
//        {             1000,              1,        1000,    1000000,    60000000,    3600000000,    86400000000,    31536000000000},
//        {          1000000,           1000,           1,       1000,       60000,       3600000,       86400000,       31536000000},
//        {       1000000000,        1000000,        1000,          1,          60,          3600,         864000,          31536000},
//        {      60000000000,       60000000,       60000,         60,           1,            60,           1440,            525600},
//        {    3600000000000,     3600000000,     3600000,       3600,          60,             1,             24,              8760},
//        {   86400000000000,    86400000000,    86400000,     864000,        1440,            24,              1,               365},
//        {31536000000000000, 31536000000000, 31536000000,   31536000,      525600,          8760,            365,                 1}
//};

uint32_t TimeProvider::convert(uint32_t value, TimeProvider::TimeUnit from, TimeProvider::TimeUnit to) {
    if (from == to) {
        return value;
    }

    if (from > to) {
        return conversionMap[from][to] * value;
    }

    return value / conversionMap[from][to];
}

void TimeProvider::Time::addNanos(uint32_t value) {
    fraction += value;

    if (fraction >= 1000000000) {
        fraction -= 1000000000;
        seconds++;
    }
}

void TimeProvider::Time::addSeconds(uint32_t value) {
    this->seconds += seconds;
}

bool TimeProvider::Time::operator>(const TimeProvider::Time &other) const {
    if (seconds > other.seconds) {
        return true;
    }

    return seconds == other.seconds && fraction > other.fraction;
}

bool TimeProvider::Time::operator>=(const TimeProvider::Time &other) const {
    if (seconds > other.seconds) {
        return true;
    }

    return seconds == other.seconds && fraction >= other.fraction;
}

bool TimeProvider::Time::operator<(const TimeProvider::Time &other) const {
    if (seconds < other.seconds) {
        return true;
    }

    return seconds == other.seconds && fraction < other.fraction;
}

bool TimeProvider::Time::operator<=(const TimeProvider::Time &other) const {
    if (seconds < other.seconds) {
        return true;
    }

    return seconds == other.seconds && fraction <= other.fraction;
}

uint32_t TimeProvider::Time::toNanos() const {
    return seconds * 1000000000 + fraction;
}

uint32_t TimeProvider::Time::toMicros() const {
    return seconds * 1000000 + fraction / 1000;
}

uint32_t TimeProvider::Time::toMillis() const {
    return seconds * 1000 + fraction / 1000000;
}

uint32_t TimeProvider::Time::toSeconds() const {
    return seconds;
}

uint32_t TimeProvider::Time::toMinutes() const {
    return seconds / 60;
}

uint32_t TimeProvider::Time::toHours() const {
    return seconds / 3600;
}

uint32_t TimeProvider::Time::toDays() const {
    return seconds / 86400;
}

uint32_t TimeProvider::Time::toYears() const {
    return seconds / 31536000;
}

}

