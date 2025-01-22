/*
 * Copyright (C) 2018-2024 Heinrich-Heine-Universitaet Duesseldorf,
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

Timestamp Timestamp::ofSeconds(uint32_t seconds) {
    return Timestamp(seconds, 0);
}

Timestamp Timestamp::ofMilliseconds(uint64_t milliseconds) {
    auto seconds = milliseconds / 1000;
    auto fraction = (milliseconds % 1000) * 1000000;
    return Timestamp(static_cast<uint32_t>(seconds), static_cast<uint32_t>(fraction));
}

Timestamp Timestamp::ofMicroseconds(uint64_t microseconds) {
    auto seconds = microseconds / 1000000;
    auto fraction = (microseconds % 1000000) * 1000;
    return Timestamp(static_cast<uint32_t>(seconds), static_cast<uint32_t>(fraction));
}

Timestamp Timestamp::ofNanoseconds(uint64_t nanoseconds) {
    auto seconds = nanoseconds / 1000000000;
    auto fraction = nanoseconds % 1000000000;
    return Timestamp(static_cast<uint32_t>(seconds), static_cast<uint32_t>(fraction));
}

Timestamp Timestamp::operator+(const Util::Time::Timestamp &other) const {
    Timestamp ret;

    if (fraction + other.fraction < NANOSECONDS_PER_SECOND) {
        ret = Timestamp(seconds + other.seconds, fraction + other.fraction);
    } else {
        // Handle fraction overflow
        ret = Timestamp(seconds + other.seconds + 1, fraction + other.fraction - NANOSECONDS_PER_SECOND);
    }

    // Handle seconds overflow
    if (ret.seconds < seconds) {
        ret = Timestamp(UINT32_MAX, UINT32_MAX);
    }

    return ret;
}

Timestamp Timestamp::operator-(const Timestamp &other) const {
    Timestamp ret;

    if (fraction - other.fraction < fraction) {
        ret = Timestamp(seconds - other.seconds, fraction - other.fraction);
    } else {
        // Handle fraction underflow
        ret = Timestamp(seconds - other.seconds - 1, NANOSECONDS_PER_SECOND - (other.fraction - fraction));
    }

    // Handle seconds underflow
    if (ret.seconds > seconds) {
        ret = Timestamp(0, 0);
    }

    return ret;
}

Timestamp &Timestamp::operator+=(const Timestamp &other) {
    auto oldSeconds = seconds;

    if (fraction + other.fraction < NANOSECONDS_PER_SECOND) {
        seconds += other.seconds;
        fraction += other.fraction;
    } else {
        // Handle fraction overflow
        seconds += other.seconds + 1;
        fraction = fraction + other.fraction - NANOSECONDS_PER_SECOND;
    }

    // Handle seconds overflow
    if (seconds < oldSeconds) {
        seconds = UINT32_MAX;
        fraction = UINT32_MAX;
    }

    return *this;
}

Timestamp &Timestamp::operator-=(const Timestamp &other) {
    auto oldSeconds = seconds;

    if (fraction - other.fraction < fraction) {
        seconds -= other.seconds;
        fraction -= other.fraction;
    } else {
        // Handle fraction overflow
        seconds -= other.seconds - 1;
        fraction = NANOSECONDS_PER_SECOND - (other.fraction - fraction);
    }

    // Handle seconds overflow
    if (seconds > oldSeconds) {
        seconds = 0;
        fraction = 0;
    }

    return *this;
}

bool Timestamp::operator==(const Timestamp &other) const {
    return seconds == other.seconds && fraction == other.fraction;
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

uint64_t Timestamp::toNanoseconds() const {
    return static_cast<uint64_t>(seconds) * 1000000000 + fraction;
}

uint64_t Timestamp::toMicroseconds() const {
    return static_cast<uint64_t>(seconds) * 1000000 + fraction / 1000;
}

uint64_t Timestamp::toMilliseconds() const {
    return static_cast<uint64_t>(seconds) * 1000 + fraction / 1000000;
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

void Timestamp::reset() {
    seconds = 0;
    fraction = 0;
}

Timestamp getSystemTime() {
    return ::getSystemTime();
}

}