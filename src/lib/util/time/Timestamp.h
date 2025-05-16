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

#ifndef HHUOS_TIMESTAMP_H
#define HHUOS_TIMESTAMP_H

#include <stdint.h>

namespace Util::Time {

class Timestamp {

public:

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

    static Timestamp ofSeconds(uint32_t seconds);

    static Timestamp ofMilliseconds(uint64_t milliseconds);

    static Timestamp ofMicroseconds(uint64_t microseconds);

    static Timestamp ofNanoseconds(uint64_t nanoseconds);

    Timestamp operator+(const Timestamp &other) const;

    Timestamp operator-(const Timestamp &other) const;

    Timestamp& operator+=(const Timestamp &other);

    Timestamp& operator-=(const Timestamp &other);

    bool operator==(const Timestamp &other) const;

    bool operator>(const Timestamp &other) const;

    bool operator>=(const Timestamp &other) const;

    bool operator<(const Timestamp &other) const;

    bool operator<=(const Timestamp &other) const;

    [[nodiscard]] uint64_t toNanoseconds() const;

    [[nodiscard]] uint64_t toMicroseconds() const;

    [[nodiscard]] uint64_t toMilliseconds() const;

    [[nodiscard]] uint32_t toSeconds() const;

    [[nodiscard]] uint32_t toMinutes() const;

    [[nodiscard]] uint32_t toHours() const;

    [[nodiscard]] uint32_t toDays() const;

    [[nodiscard]] uint32_t toYears() const;

    void reset();

private:

    uint32_t seconds = 0;
    uint32_t fraction = 0;

    static const constexpr uint32_t NANOSECONDS_PER_SECOND = 1000000000;

};

Timestamp getSystemTime();

}

#endif