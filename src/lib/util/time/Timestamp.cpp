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

#include "Timestamp.h"
#include "lib/interface.h"

namespace Util::Time {

Timestamp::Timestamp(uint32_t seconds, uint32_t fraction = 0) : seconds(seconds), fraction(fraction) {}

uint32_t Timestamp::convert(uint32_t value, Timestamp::TimeUnit from, Timestamp::TimeUnit to) {
    if (from == to) {
        return value;
    }

    if (from > to) {
        return conversionMap[from][to] * value;
    }

    return value / conversionMap[from][to];
}

void Timestamp::addNanoseconds(uint32_t value) {
    fraction += value;

    while (fraction >= 1000000000) {
        fraction -= 1000000000;
        seconds++;
    }
}

void Timestamp::addSeconds(uint32_t value) {
    this->seconds += seconds;
}

bool Timestamp::operator>(const Timestamp &other) const {
    if (seconds > other.seconds) {
        return true;
    }

    return seconds == other.seconds && fraction > other.fraction;
}

bool Timestamp::operator>=(const Timestamp &other) const {
    if (seconds > other.seconds) {
        return true;
    }

    return seconds == other.seconds && fraction >= other.fraction;
}

bool Timestamp::operator<(const Timestamp &other) const {
    if (seconds < other.seconds) {
        return true;
    }

    return seconds == other.seconds && fraction < other.fraction;
}

bool Timestamp::operator<=(const Timestamp &other) const {
    if (seconds < other.seconds) {
        return true;
    }

    return seconds == other.seconds && fraction <= other.fraction;
}

uint32_t Timestamp::toNanoseconds() const {
    return seconds * 1000000000 + fraction;
}

uint32_t Timestamp::toMicroseconds() const {
    return seconds * 1000000 + fraction / 1000;
}

uint32_t Timestamp::toMilliseconds() const {
    return seconds * 1000 + fraction / 1000000;
}

uint32_t Timestamp::toSeconds() const {
    return seconds;
}

uint32_t Timestamp::toMinutes() const {
    return seconds / 60;
}

uint32_t Timestamp::toHours() const {
    return seconds / 3600;
}

uint32_t Timestamp::toDays() const {
    return seconds / 86400;
}

uint32_t Timestamp::toYears() const {
    return seconds / 31536000;
}

Timestamp Timestamp::ofMilliseconds(uint32_t milliseconds) {
    auto seconds = milliseconds / 1000;
    auto fraction = (milliseconds % 1000) * 1000000;
    return {seconds, fraction};
}

Timestamp getSystemTime() {
    return ::getSystemTime();
}

}