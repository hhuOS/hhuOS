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

#ifndef __Address_include__
#define __Address_include__

#include <cstdint>
#include <lib/async/Atomic.h>

namespace Util {

template<typename T>
class Address {

public:

    Address();

    explicit Address(T address);

    ~Address() = default;

    Address(const Address &other) = default;

    Address &operator=(const Address &other) = default;

    bool operator!=(const Address &other) const;

    bool operator==(const Address &other) const;

    explicit operator T() const;

    [[nodiscard]] T get() const;

    [[nodiscard]] Address<T> add(T value) const;

    [[nodiscard]] Address<T> subtract(T value) const;

    [[nodiscard]] Address alignUp(T alignment) const;

private:

    T address;
};

template
class Address<uint8_t>;

template
class Address<uint16_t>;

template
class Address<uint32_t>;

}

#endif
