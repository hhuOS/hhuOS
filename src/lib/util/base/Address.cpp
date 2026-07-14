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

#include "Address.h"

namespace Util {

size_t Address::stringLength() const {
    const auto *pointer = reinterpret_cast<char*>(address);

    size_t i;
    for (i = 0; pointer[i] != '\0'; i++) {}

    return i;
}

void Address::setRange(const uint8_t value, const size_t length) const {
    auto *target = reinterpret_cast<uint8_t*>(address);

    for (size_t i = 0; i < length; ++i) {
        target[i] = value;
    }
}

void Address::copyRange(const Address &sourceAddress, const size_t length) const {
    auto * __restrict__ target = reinterpret_cast<uint8_t*>(address);
    const auto * __restrict__ source = reinterpret_cast<const uint8_t*>(sourceAddress.get());

    for (size_t i = 0; i < length; ++i) {
        target[i] = source[i];
    }
}

void Address::copyRange(const void *sourceAddress, const size_t length) const {
    copyRange(Address(sourceAddress), length);
}

void Address::copyString(const Address &sourceAddress) const {
    auto *target = reinterpret_cast<char*>(address);
    const auto *source = reinterpret_cast<char*>(sourceAddress.address);

    while (*source != 0) {
        *target++ = *source++;
    }
    *target = 0;
}

void Address::copyString(const char *sourceString) const {
    copyString(Address(sourceString));
}

void Address::copyString(const Address &sourceAddress, const size_t maxBytes) const {
    auto *target = reinterpret_cast<char*>(address);
    const auto *source = reinterpret_cast<char*>(sourceAddress.address);

    size_t i;
    for (i = 0; source[i] != 0 && i < maxBytes; i++) {
        target[i] = source[i];
    }
    target[i] = 0;

    for (; i < maxBytes; i++) {
        target[i] = 0;
    }
}

int32_t Address::compareRange(const Address &otherAddress, const size_t length) const {
    const auto *pointer = reinterpret_cast<char*>(address);
    const auto *other = reinterpret_cast<char*>(otherAddress.address);

    size_t i = 0;
    while (i < length && pointer[i] == other[i]) {
        i++;
    }

    return i == length ? 0 : pointer[i] - other[i];
}

int32_t Address::compareString(const Address &otherAddress) const {
    const auto *pointer = reinterpret_cast<char*>(address);
    const auto *other = reinterpret_cast<char*>(otherAddress.address);

    size_t i = 0;
    while (pointer[i] != 0 && other[i] != 0 && pointer[i] == other[i]) {
        i++;
    }

    return  pointer[i] - other[i];
}

Address Address::searchCharacter(const char character) const {
    const auto *pointer = reinterpret_cast<char*>(address);

    size_t i = 0;
    while (pointer[i] != 0 && pointer[i] != character) {
        i++;
    }

    return pointer[i] == 0 ? Address(static_cast<size_t>(0)) : add(i);
}

}
