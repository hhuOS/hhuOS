/*
 * Copyright (C) 2017-2025 Heinrich Heine University Düsseldorf,
 * Institute of Computer Science, Department Operating Systems
 * Main developers: Christian Gesse <christian.gesse@hhu.de>, Fabian Ruhland <ruhland@hhu.de>
 * Original development team: Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schöttner
 * This project has been supported by several students.
 * A full list of integrated student theses can be found here: https://github.com/hhuOS/hhuOS/wiki/Student-theses
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

Timestamp::Timestamp(const size_t seconds, const uint32_t fraction) : seconds(seconds), fraction(fraction) {}

Timestamp Timestamp::getSystemTime() {
    return ::getSystemTime();
}

Timestamp Timestamp::ofSeconds(const size_t seconds) {
    return Timestamp(seconds, 0);
}

Timestamp Timestamp::ofMilliseconds(const uint64_t milliseconds) {
    const auto seconds = milliseconds / 1000;
    const auto fraction = (milliseconds % 1000) * 1000000;

    return Timestamp(seconds, fraction);
}

Timestamp Timestamp::ofMicroseconds(const uint64_t microseconds) {
    const auto seconds = microseconds / 1000000;
    const auto fraction = (microseconds % 1000000) * 1000;

    return Timestamp(seconds, fraction);
}

Timestamp Timestamp::ofNanoseconds(const uint64_t nanoseconds) {
    const auto seconds = nanoseconds / 1000000000;
    const auto fraction = nanoseconds % 1000000000;

    return Timestamp(seconds, fraction);
}

Timestamp Timestamp::operator+(const Timestamp &other) const {
    size_t newSeconds;
    uint32_t newFraction;

    if (fraction + other.fraction < NANOSECONDS_PER_SECOND) {
        newSeconds = seconds + other.seconds;
        newFraction = fraction + other.fraction;
    } else {
        // Handle fraction overflow
        newSeconds = seconds + other.seconds + 1;
        newFraction = fraction + other.fraction - NANOSECONDS_PER_SECOND;
    }

    // Handle seconds overflow
    if (newSeconds < seconds) {
        newSeconds = SIZE_MAX;
        newFraction = UINT32_MAX;
    }

    return Timestamp(newSeconds, newFraction);
}

Timestamp Timestamp::operator-(const Timestamp &other) const {
    size_t newSeconds;
    uint32_t newFraction;

    if (fraction - other.fraction < fraction) {
        newSeconds = seconds - other.seconds;
        newFraction = fraction - other.fraction;
    } else {
        // Handle fraction underflow
        newSeconds = seconds - other.seconds - 1;
        newFraction = NANOSECONDS_PER_SECOND - (other.fraction - fraction);
    }

    // Handle seconds underflow
    if (newSeconds > seconds) {
        newSeconds = 0;
        newFraction = 0;
    }

    return Timestamp(newSeconds, newFraction);
}

Timestamp &Timestamp::operator+=(const Timestamp &other) {
    const auto oldSeconds = seconds;

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
    const auto oldSeconds = seconds;

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

bool Timestamp::operator!=(const Timestamp &other) const {
    return seconds != other.seconds || fraction != other.fraction;
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

}