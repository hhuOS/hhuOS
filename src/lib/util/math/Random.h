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

#ifndef HHUOS_LIB_UTIL_RANDOM_H
#define HHUOS_LIB_UTIL_RANDOM_H

#include <stdint.h>

#include "util/time/Timestamp.h"

namespace Util::Math {

/// A simple random number generator based on the multiplicative linear congruential generator (LCG) algorithm.
/// It can be initialized with a custom seed, multiplier, increment, and modulus or with default values
/// corresponding to the minimal standard LCG (MINSTD).
///
/// It generates 32-bit pseudo-random numbers in the range of [0, modulus) using the formula:
/// ```c++
/// randomNumber = (multiplier * randomNumber + increment) % modulus
/// ```
///
/// For the MINSTD generator, the default values are:
/// multiplier = 48271, increment = 0, modulus = 2147483647 (see https://cplusplus.com/reference/random/minstd_rand/)
///
/// ## Example
/// ```c++
/// auto random = Util::Math::Random(); // Default generator with system time as seed
/// auto minstd = Util::Math::Random(1); // MINSTD generator with starting seed 1
/// auto custom = Util::Math::Random(5, 3, 3, 7); // Custom generator
///
/// const auto randomNumber = random.getRandomNumber(1, 10); // Get a random number in the range [1, 10]
/// const auto randomDouble = random.getRandomNumber(); // Get a random floating point number in the range [0.0, 1.0)
/// ```
class Random {

public:
    /// Create a new random number generator with custom parameters for the LCG algorithm.
    Random(uint32_t seed, uint32_t multiplier, uint32_t increment, uint32_t modulus);

    /// Create a new random number generator with default parameters for the MINSTD LCG algorithm.
    /// This constructor can be used without parameters, in which case the seed is set to the current system time.
    explicit Random(uint32_t seed = Time::Timestamp::getSystemTime().toMilliseconds());

    /// Get a random number in the range [min, max].
    ///
    /// ### Example
    /// ```c++
    /// auto random = Util::Math::Random();
    ///
    /// // Roll a die with 6 sides
    /// const auto d6Roll = random.getRandomNumber(1, 6);
    ///
    /// // Roll a die with 20 sides
    /// const auto d20Roll = random.getRandomNumber(1, 20);
    ///
    /// // Get a random number in the range [10, 50]
    /// const auto randomNumber = random.getRandomNumber(10, 50);
    /// ```
    uint32_t getRandomNumber(uint32_t min, uint32_t max);

    /// Get a random floating point number in the range [0.0, 1.0).
    ///
    /// ### Example
    /// ```c++
    /// auto random = Util::Math::Random();
    ///
    /// // Get a random floating point number in the range [0.0, 1.0)
    /// const auto randomDouble = random.getRandomNumber();
    ///
    /// // Get a random floating point number in the range [0.0, 0.5)
    /// const auto randomHalf = random.getRandomNumber() * 0.5;
    ///
    /// // Get a random floating point number in the range [2.0, 10.0)
    /// const auto randomRange = 2.0 + random.getRandomNumber() * (10.0 - 2.0);
    /// ```
    double getRandomNumber();

private:

    /// Calculate the next random number using the LCG formula.
    /// This method updates the internal state of the generator and returns the next random number.
    /// It is called internally by the public methods to generate random numbers.
    uint32_t nextRandomNumber();

    uint32_t randomNumber;
    const uint32_t multiplier;
    const uint32_t increment;
    const uint32_t modulus;

    static constexpr uint32_t MINSTD_MULTIPLIER = 48271;
    static constexpr uint32_t MINSTD_INCREMENT = 0;
    static constexpr uint32_t MINSTD_MODULUS = 2147483647;
};

}

#endif
