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

void Address::setRange(const uint8_t value, size_t length) const {
    // Just set very small memory blocks byte by byte
    if (length < 16) {
        auto target = reinterpret_cast<uint8_t*>(address);
        while (length-- > 0) {
            *target++ = value;
        }

        return;
    }

    // Variables needed to fill the bytes up to the next 8-byte aligned address

    // Number of bytes to next 8-byte aligned address
    auto alignDifference = address % 8;
    // Start of the memory block (used to fill the bytes before the 8-byte aligned address)
    auto beforeAlign = reinterpret_cast<uint8_t*>(address);

    // Variables needed to fill the 8-byte aligned memory blocks

    // Number of 8-byte blocks to fill
    auto remainingBlocks = (length - alignDifference) / 8;
    // Start of the 8-byte aligned memory block
    auto *target = reinterpret_cast<uint64_t*>(address + alignDifference);
    // 8-byte value to fill the memory block
    const auto longValue = static_cast<uint64_t>(value) |
                           static_cast<uint64_t>(value) << 8 |
                           static_cast<uint64_t>(value) << 16 |
                           static_cast<uint64_t>(value) << 24 |
                           static_cast<uint64_t>(value) << 32 |
                           static_cast<uint64_t>(value) << 40 |
                           static_cast<uint64_t>(value) << 48 |
                           static_cast<uint64_t>(value) << 56;

    // Variables needed to fill the remaining bytes

    // Number of remaining bytes to fill
    auto remainingBytes = (length - alignDifference) % 8;
    // Start of the remaining memory block (used to fill the remaining bytes)
    auto *rest = reinterpret_cast<uint8_t*>(target) + remainingBlocks * 8;

    // First fill the bytes up to the next 8-byte aligned address
    while (alignDifference-- > 0) {
        *beforeAlign++ = value;
    }

    // Now fill the 8-byte aligned memory blocks
    while (remainingBlocks-- > 0) {
        *target++ = longValue;
    }

    // Finally fill the remaining bytes
    while (remainingBytes-- > 0) {
        *rest++ = value;
    }
}

void Address::copyRange(const Address &sourceAddress, size_t length) const {
    // Just copy very small memory blocks byte by byte
    if (length < 16) {
        auto source = reinterpret_cast<uint8_t*>(sourceAddress.get());
        auto target = reinterpret_cast<uint8_t*>(address);
        while (length-- > 0) {
            *target++ = *source++;
        }

        return;
    }

    // Variables needed to fill the bytes up to the next 8-byte aligned address

    // Number of bytes to next 8-byte aligned address
    auto alignDifference = address % 8;
    // Start of the source memory block (used to copy the bytes before the 8-byte aligned address)
    auto beforeAlignSource = reinterpret_cast<uint8_t*>(sourceAddress.get());
    // Start of the target memory block (used to copy the bytes before the 8-byte aligned address)
    auto beforeAlign = reinterpret_cast<uint8_t*>(address);

    // Variables needed to fill the 8-byte aligned memory blocks

    // Number of 8-byte blocks to fill
    auto remainingBlocks = (length - alignDifference) / 8;
    // Start of the 8-byte aligned source memory block
    auto *source = reinterpret_cast<uint64_t*>(sourceAddress.get() + alignDifference);
    // Start of the 8-byte aligned target memory block
    auto *target = reinterpret_cast<uint64_t*>(address + alignDifference);

    // Variables needed to fill the remaining bytes

    // Number of remaining bytes to fill
    auto remainingBytes = (length - alignDifference) % 8;
    // Start of the remaining source memory block (used to copy the remaining bytes)
    const auto *restSource = reinterpret_cast<uint8_t*>(source) + remainingBlocks * 8;
    // Start of the remaining target memory block (used to copy the remaining bytes)
    auto *rest = reinterpret_cast<uint8_t*>(target) + remainingBlocks * 8;

    // First fill the bytes up to the next 8-byte aligned address
    while (alignDifference-- > 0) {
        *beforeAlign++ = *beforeAlignSource++;
    }

    // Now fill the 8-byte aligned memory blocks
    while (remainingBlocks-- > 0) {
        *target++ = *source++;
    }

    // Finally fill the remaining bytes
    while (remainingBytes-- > 0) {
        *rest++ = *restSource++;
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