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

#ifndef HHUOS_LIB_UTIL_PAIR_H
#define HHUOS_LIB_UTIL_PAIR_H

#include <stddef.h>

namespace Util {

/// A simple pair class that holds two values of potentially different types.
/// This class is immutable, meaning that once a Pair is created, its values cannot be changed.
///
/// ### Example
/// ```c++
/// const auto pair = Util::Pair<int, float>(42, 3.14f);
/// cont auto first = pair.getFirst(); // 42
/// const auto second = pair.getSecond(); // 3.14f
/// ```
template<typename T, typename U>
class Pair {

public:
    /// Create a new Pair instance with default values.
    Pair() = default;

    /// Create a new Pair instance with the given values.
    /// The values are copied into the Pair.
    Pair(T first, U second);

    /// The Pair destructor is trivial, as this class only contains the two values.
    ~Pair() = default;

    /// The Pair copy constructor is trivial, as this class only contains the two values.
    Pair(const Pair &other) = default;

    /// The Pair assignment operator is trivial, as this class only contains the two values.
    Pair &operator=(const Pair &other) = default;

    /// Compare this Pair with another Pair for inequality.
    /// This is done by comparing both the first and second values.
    bool operator!=(const Pair &other) const;

    /// Compare this Pair with another Pair for equality.
    /// This is done by comparing both the first and second values.
    bool operator==(const Pair &other) const;

    /// Access the first value of the Pair.
    const T& getFirst() const;

    /// Access the second value of the Pair.
    const U& getSecond() const;

private:

    T first;
    U second;
};

template<typename T, typename U>
Pair<T, U>::Pair(T first, U second) : first(first), second(second) {}

template<typename T, typename U>
bool Pair<T, U>::operator!=(const Pair &other) const {
    return first != other.first || second != other.second;

}

template<typename T, typename U>
bool Pair<T, U>::operator==(const Pair &other) const {
    return first == other.first && second == other.second;
}

template<typename T, typename U>
const T & Pair<T, U>::getFirst() const {
    return first;
}

template<typename T, typename U>
const U & Pair<T, U>::getSecond() const {
    return second;
}

}


#endif
