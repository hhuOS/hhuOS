/*
 * Copyright (C) 2017-2026 Heinrich Heine University Düsseldorf,
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

#include "interface.h"

namespace Util {
namespace Time {

Timestamp Timestamp::getSystemTime() {
    return ::getSystemTime();
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
        // Handle fraction underflow
        seconds -= other.seconds + 1;
        fraction = NANOSECONDS_PER_SECOND - (other.fraction - fraction);
    }

    // Handle seconds underflow
    if (seconds > oldSeconds) {
        seconds = 0;
        fraction = 0;
    }

    return *this;
}

}
}