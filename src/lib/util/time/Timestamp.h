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

#ifndef HHUOS_LIB_UTIL_TIME_TIMESTAMP_H
#define HHUOS_LIB_UTIL_TIME_TIMESTAMP_H

#include <stdint.h>
#include <stddef.h>

namespace Util::Time {

/// Represents a timestamp with a resolution of nanoseconds.
/// The timestamp is composed of a number of seconds and a fraction in nanoseconds.
class Timestamp {

public:
    /// Create a new Timestamp instance with 0 seconds and 0 nanoseconds.
    Timestamp() = default;

    /// Create a new Timestamp instance with the specified seconds and fraction.
    explicit Timestamp(size_t seconds, uint32_t fraction = 0);

    /// Timestamp only contains primitive types, so the default destructor is sufficient.
    ~Timestamp() = default;

    /// Get the current system time (time since boot) as a Timestamp.
    ///
    /// ### Example
    /// ```c++
    /// const auto start = Util::Time::Timestamp::getSystemTime();
    /// const auto end = start + Util::Time::Timestamp::ofSeconds(5);
    ///
    /// // Execute a loop for 5 seconds
    /// // (if you just want to wait consider using `Util::Async::Thread::sleep()` instead to save CPU cycles).
    /// while (Util::Time::Timestamp::getSystemTime() < end) {
    ///     // Do something...
    /// }
    /// ```
    static Timestamp getSystemTime();

    /// Create a Timestamp from the given number of seconds.
    ///
    /// ### Example
    /// ```c++
    /// const auto timestamp = Util::Time::Timestamp::ofSeconds(5);
    ///
    /// const auto seconds = timestamp.toSeconds(); // seconds = 5
    /// const auto nanoseconds = timestamp.toNanoseconds(); // nanoseconds = 5000000000
    /// ```
    static Timestamp ofSeconds(size_t seconds);

    /// Create a Timestamp from the given number of milliseconds.
    ///
    /// ### Example
    /// ```c++
    /// const auto timestamp = Util::Time::Timestamp::ofMilliseconds(1797);
    ///
    /// const auto seconds = timestamp.toSeconds(); // seconds = 1
    /// const auto nanoseconds = timestamp.toNanoseconds(); // nanoseconds = 1797000000
    /// ```
    static Timestamp ofMilliseconds(uint64_t milliseconds);

    /// Create a Timestamp from the given number of microseconds.
    ///
    /// ### Example
    /// ```c++
    /// const auto timestamp = Util::Time::Timestamp::ofMicroseconds(123456789);
    ///
    /// const auto seconds = timestamp.toSeconds(); // seconds = 123
    /// const auto nanoseconds = timestamp.toNanoseconds(); // nanoseconds = 123456789000
    /// ```
    static Timestamp ofMicroseconds(uint64_t microseconds);

    /// Create a Timestamp from the given number of nanoseconds.
    ///
    /// ### Example
    /// ```c++
    /// const auto timestamp = Util::Time::Timestamp::ofNanoseconds(9876543210);
    ///
    /// const auto seconds = timestamp.toSeconds(); // seconds = 9
    /// const auto nanoseconds = timestamp.toNanoseconds(); // nanoseconds = 9876543210
    /// ```
    static Timestamp ofNanoseconds(uint64_t nanoseconds);

    /// Add two timestamps together.
    ///
    /// ### Example
    /// ```c++
    /// const auto timestamp1 = Util::Time::Timestamp::ofSeconds(5); // timestamp1 = 5 seconds
    /// const auto timestamp2 = Util::Time::Timestamp::ofMilliseconds(500); // timestamp2 = 0.5 seconds
    ///
    /// const auto result = timestamp1 + timestamp2; // result = 5.5 seconds
    /// const auto resultMilliseconds = result.toMilliseconds(); // resultMilliseconds = 5500
    /// ```
    Timestamp operator+(const Timestamp &other) const;

    /// Subtract one timestamp from another.
    ///
    /// ### Example
    /// ```c++
    /// const auto timestamp1 = Util::Time::Timestamp::ofSeconds(5); // timestamp1 = 5 seconds
    /// const auto timestamp2 = Util::Time::Timestamp::ofMilliseconds(500); // timestamp2 = 0.5 seconds
    ///
    /// const auto result = timestamp1 - timestamp2; // result = 4.5 seconds
    /// const auto resultMilliseconds = result.toMilliseconds(); // resultMilliseconds = 4500
    /// ```
    Timestamp operator-(const Timestamp &other) const;

    /// Add another timestamp to this one.
    ///
    /// ### Example
    /// ```c++
    /// const auto timestamp1 = Util::Time::Timestamp::ofSeconds(5); // timestamp1 = 5 seconds
    /// auto timestamp2 = Util::Time::Timestamp::ofMilliseconds(500); // timestamp2 = 0.5 seconds
    ///
    /// timestamp2 += timestamp1; // timestamp2 = 5.5 seconds
    /// const auto milliseconds = timestamp2.toMilliseconds(); // milliseconds = 5500
    /// ```
    Timestamp& operator+=(const Timestamp &other);

    /// Subtract another timestamp from this one.
    ///
    /// ### Example
    /// ```c++
    /// const auto timestamp1 = Util::Time::Timestamp::ofSeconds(5); // timestamp1 = 5 seconds
    /// auto timestamp2 = Util::Time::Timestamp::ofMilliseconds(500); // timestamp2 = 0.5 seconds
    ///
    /// timestamp2 -= timestamp1; // timestamp2 = 4.5 seconds
    /// const auto milliseconds = timestamp2.toMilliseconds(); // milliseconds = 4500
    /// ```
    Timestamp& operator-=(const Timestamp &other);

    /// Compare two timestamps for equality.
    ///
    /// ### Example
    /// ```c++
    /// const auto timestamp1 = Util::Time::Timestamp::ofSeconds(5); // timestamp1 = 5 seconds
    /// const auto timestamp2 = Util::Time::Timestamp::ofMilliseconds(5000); // timestamp2 = 5 seconds
    /// const auto timestamp3 = Util::Time::Timestamp::ofSeconds(6); // timestamp3 = 6 seconds
    ///
    /// const auto isEqual1 = (timestamp1 == timestamp2); // isEqual1 = true
    /// const auto isEqual2 = (timestamp1 == timestamp3); // isEqual2 = false
    /// ```
    bool operator==(const Timestamp &other) const;

    /// Compare two timestamps for inequality.
    ///
    /// ### Example
    /// ```c++
    /// const auto timestamp1 = Util::Time::Timestamp::ofSeconds(5); // timestamp1 = 5 seconds
    /// const auto timestamp2 = Util::Time::Timestamp::ofMilliseconds(5000); // timestamp2 = 5 seconds
    /// const auto timestamp3 = Util::Time::Timestamp::ofSeconds(6); // timestamp3 = 6 seconds
    ///
    /// const auto isNotEqual1 = (timestamp1 != timestamp2); // isNotEqual1 = false
    /// const auto isNotEqual2 = (timestamp1 != timestamp3); // isNotEqual2 = true
    /// ```
    bool operator!=(const Timestamp &other) const;

    /// Check if this timestamp is greater than another.
    ///
    /// ### Example
    /// ```c++
    /// const auto timestamp1 = Util::Time::Timestamp::ofSeconds(5); // timestamp1 = 5 seconds
    /// const auto timestamp2 = Util::Time::Timestamp::ofMilliseconds(5000); // timestamp2 = 5 seconds
    /// const auto timestamp3 = Util::Time::Timestamp::ofSeconds(6); // timestamp3 = 6 seconds
    ///
    /// const auto isGreater1 = (timestamp1 > timestamp2); // isGreater1 = false
    /// const auto isGreater2 = (timestamp1 > timestamp3); // isGreater2 = false
    /// const auto isGreater3 = (timestamp3 > timestamp1); // isGreater3 = true
    /// ```
    bool operator>(const Timestamp &other) const;
    
    /// Check if this timestamp is greater than or equal to another.
    ///
    /// ### Example
    /// ```c++
    /// const auto timestamp1 = Util::Time::Timestamp::ofSeconds(5); // timestamp1 = 5 seconds
    /// const auto timestamp2 = Util::Time::Timestamp::ofMilliseconds(5000); // timestamp2 = 5 seconds
    /// const auto timestamp3 = Util::Time::Timestamp::ofSeconds(6); // timestamp3 = 6 seconds
    ///
    /// const auto isGreaterOrEqual1 = (timestamp1 >= timestamp2); // isGreaterOrEqual1 = true
    /// const auto isGreaterOrEqual2 = (timestamp1 >= timestamp3); // isGreaterOrEqual2 = false
    /// const auto isGreaterOrEqual3 = (timestamp3 >= timestamp1); // isGreaterOrEqual3 = true
    /// ```
    bool operator>=(const Timestamp &other) const;

    /// Check if this timestamp is less than another.
    ///
    /// ### Example
    /// ```c++
    /// const auto timestamp1 = Util::Time::Timestamp::ofSeconds(5); // timestamp1 = 5 seconds
    /// const auto timestamp2 = Util::Time::Timestamp::ofMilliseconds(5000); // timestamp2 = 5 seconds
    /// const auto timestamp3 = Util::Time::Timestamp::ofSeconds(6); // timestamp3 = 6 seconds
    ///
    /// const auto isLess1 = (timestamp1 < timestamp2); // isLess1 = false
    /// const auto isLess2 = (timestamp1 < timestamp3); // isLess2 = true
    /// const auto isLess3 = (timestamp3 < timestamp1); // isLess3 = false
    /// ```
    bool operator<(const Timestamp &other) const;
    
    /// Check if this timestamp is less than or equal another.
    ///
    /// ### Example
    /// ```c++
    /// const auto timestamp1 = Util::Time::Timestamp::ofSeconds(5); // timestamp1 = 5 seconds
    /// const auto timestamp2 = Util::Time::Timestamp::ofMilliseconds(5000); // timestamp2 = 5 seconds
    /// const auto timestamp3 = Util::Time::Timestamp::ofSeconds(6); // timestamp3 = 6 seconds
    ///
    /// const auto isLessOrEqual1 = (timestamp1 <= timestamp2); // isLessOrEqual1 = true
    /// const auto isLessOrEqual2 = (timestamp1 <= timestamp3); // isLessOrEqual2 = true
    /// const auto isLessOrEqual3 = (timestamp3 <= timestamp1); // isLessOrEqual3 = false
    /// ```
    bool operator<=(const Timestamp &other) const;

    /// Convert the timestamp to nanoseconds.
    ///
    /// ### Example
    /// ```c++
    /// const auto timestamp1 = Util::Time::Timestamp::ofSeconds(5);
    /// const auto timestamp2 = Util::Time::Timestamp::ofMilliseconds(3);
    ///
    /// const auto nanoseconds1 = timestamp1.toNanoseconds(); // nanoseconds1 = 5000000000
    /// const auto nanoseconds2 = timestamp2.toNanoseconds(); // nanoseconds2 = 3000000
    /// ```
    [[nodiscard]] uint64_t toNanoseconds() const;
    
    /// Convert the timestamp to microseconds.
    ///
    /// ### Example
    /// ```c++
    /// const auto timestamp1 = Util::Time::Timestamp::ofSeconds(5);
    /// const auto timestamp2 = Util::Time::Timestamp::ofMilliseconds(3);
    ///
    /// const auto microseconds1 = timestamp1.toMicroseconds(); // microseconds1 = 5000000
    /// const auto microseconds2 = timestamp2.toMicroseconds(); // microseconds2 = 3000
    /// ```
    [[nodiscard]] uint64_t toMicroseconds() const;
    
    /// Convert the timestamp to milliseconds.
    ///
    /// ### Example
    /// ```c++
    /// const auto timestamp1 = Util::Time::Timestamp::ofSeconds(5);
    /// const auto timestamp2 = Util::Time::Timestamp::ofMilliseconds(3);
    ///
    /// const auto milliseconds1 = timestamp1.toMilliseconds(); // milliseconds1 = 5000
    /// const auto milliseconds2 = timestamp2.toMilliseconds(); // milliseconds2 = 3
    /// ```
    [[nodiscard]] uint64_t toMilliseconds() const;
    
    /// Convert the timestamp to seconds.
    ///
    /// ### Example
    /// ```c++
    /// const auto timestamp1 = Util::Time::Timestamp::ofSeconds(5);
    /// const auto timestamp2 = Util::Time::Timestamp::ofMilliseconds(3);
    ///
    /// const auto seconds1 = timestamp1.toMilliseconds(); // seconds1 = 5
    /// const auto seconds2 = timestamp2.toMilliseconds(); // seconds2 = 0
    /// ```
    [[nodiscard]] size_t toSeconds() const;

    /// Convert the timestamp to minutes.
    ///
    /// ### Example
    /// ```c++
    /// const auto timestamp1 = Util::Time::Timestamp::ofSeconds(300);
    /// const auto timestamp2 = Util::Time::Timestamp::ofSeconds(59);
    ///
    /// const auto minutes1 = timestamp1.toMinutes(); // minutes1 = 5
    /// const auto minutes2 = timestamp2.toMinutes(); // minutes2 = 0
    /// ```
    [[nodiscard]] size_t toMinutes() const;

    /// Convert the timestamp to hours.
    ///
    /// ### Example
    /// ```c++
    /// const auto timestamp1 = Util::Time::Timestamp::ofSeconds(7200);
    /// const ato timestamp2 = Util::Time::Timestamp::ofSeconds(3599);
    ///
    /// const auto hours1 = timestamp1.toHours(); // hours1 = 2
    /// const auto hours2 = timestamp2.toHours(); // hours2 = 0
    /// ```
    [[nodiscard]] size_t toHours() const;

    /// Convert the timestamp to days.
    ///
    /// ### Example
    /// ```c++
    /// const auto timestamp1 = Util::Time::Timestamp::ofSeconds(100000);
    /// const auto timestamp2 = Util::Time::Timestamp::ofSeconds(86399);
    ///
    /// const auto days1 = timestamp1.toDays(); // days1 = 1
    /// const auto days2 = timestamp2.toDays(); // days2 = 0
    /// ```
    [[nodiscard]] size_t toDays() const;

    /// Convert the timestamp to years.
    ///
    /// ### Example
    /// ```c++
    /// const auto timestamp1 = Util::Time::Timestamp::ofSeconds(50000000);
    /// const auto timestamp2 = Util::Time::Timestamp::ofSeconds(31535999);
    ///
    /// const auto years1 = timestamp1.toYears(); // years1 = 1
    /// const auto years2 = timestamp2.toYears(); // years2 = 0
    /// ```
    [[nodiscard]] size_t toYears() const;

private:

    size_t seconds = 0;
    uint32_t fraction = 0;

    static constexpr uint32_t NANOSECONDS_PER_SECOND = 1000000000;
};

}

#endif