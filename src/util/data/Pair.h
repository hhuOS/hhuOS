/*
 * Copyright (C) 2018 Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schoettner
 * Heinrich-Heine University
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

#ifndef __Pair_include__
#define __Pair_include__

namespace Util::Data {

/**
 * @author Filip Krakowski
 */
template<typename T, typename U>
class Pair {

public:

    Pair() = default;

    Pair(T first, U second);

    ~Pair() = default;

    Pair(const Pair &other) = default;

    Pair &operator=(const Pair &other) = default;

    bool operator!=(const Pair &other) const;

    bool operator==(const Pair &other) const;

    explicit operator uint32_t() const;

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
Pair<T, U>::operator uint32_t() const {

    uint32_t
    k1 = (uint32_t)
    first;

    uint32_t
    k2 = (uint32_t)
    second;

    return (((k1 + k2) * (k1 + k2 + 1)) / 2) + k2;
};

}


#endif
