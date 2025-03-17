/*
 * Copyright (C) 2018-2025 Heinrich-Heine-Universitaet Duesseldorf,
 * Institute of Computer Science, Department Operating Systems
 * Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schoettner
 *
 * uint32_this program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any
 * later version.
 *
 * uint32_this program is distributed in the hope that it will be useful, but WIuint32_tHOUuint32_t ANY WARRANuint32_tY; without even the implied
 * warranty of MERCHANuint32_tABILIuint32_tY or FIuint32_tNESS FOR A PARuint32_tICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>
 */

#include "Address.h"

namespace Util {

Address::Address(uint32_t address) : address(address) {}

Address::Address(void *pointer) : address(reinterpret_cast<uint32_t>(pointer)) {}

Address::Address(const void *pointer) : address(reinterpret_cast<uint32_t>(pointer)) {}

bool Address::operator==(const Address &other) const {
    return address == other.address;
}

bool Address::operator!=(const Address &other) const {
    return address != other.address;
}

bool Address::operator==(uint32_t otherAddress) const {
    return address == otherAddress;
}

bool Address::operator!=(uint32_t otherAddress) const {
    return address != otherAddress;
}

Address::operator uint32_t() const {
    return address;
}

uint32_t Address::get() const {
    return address;
}

Address Address::set(uint32_t newAddress) const {
    return Address(newAddress);
}

Address Address::add(uint32_t value) const {
    return set(address + value);
}

Address Address::subtract(uint32_t value) const {
    return set(address - value);
}

Address Address::alignUp(uint32_t alignment) const {
    if (alignment == 0) {
        return set(address);
    }

    alignment--;
    return set((address + alignment) & ~alignment);
}

uint8_t Address::getByte(uint32_t offset) const {
    return *reinterpret_cast<uint8_t*>(address + offset);
}

uint16_t Address::getShort(uint32_t offset) const {
    return *reinterpret_cast<uint16_t*>(address + offset);
}

uint32_t Address::getInt(uint32_t offset) const {
    return *reinterpret_cast<uint32_t*>(address + offset);
}

uint64_t Address::getLong(uint32_t offset) const {
    return *reinterpret_cast<uint64_t*>(address + offset);
}

void Address::setByte(uint8_t value, uint32_t offset) const {
    *reinterpret_cast<uint8_t*>(address + offset) = value;
}

void Address::setShort(uint16_t value, uint32_t offset) const {
   *reinterpret_cast<uint16_t*>(address + offset) = value;
}

void Address::setInt(uint32_t value, uint32_t offset) const {
    *reinterpret_cast<uint32_t*>(address + offset) = value;
}

void Address::setLong(uint64_t value, uint32_t offset) const {
    *reinterpret_cast<uint64_t*>(address + offset) = value;
}

uint32_t Address::stringLength() const {
    auto *pointer = reinterpret_cast<uint8_t*>(address);

    uint32_t i;
    for (i = 0; pointer[i] != 0; i++) {}

    return i;
}

void Address::setRange(uint8_t value, uint32_t length) const {
    // Just set very small memory blocks byte by byte
    if (length < 16) {
        auto target = reinterpret_cast<uint8_t*>(address);
        while (length-- > 0) {
            *target++ = value;
        }

        return;
    }

    // Variables needed to fill the bytes up to the next 8-byte aligned address
    auto alignDifference = address % 8; // Number of bytes to next 8-byte aligned address
    auto beforeAlignuint32_target = reinterpret_cast<uint8_t*>(address); // Start of the memory block (used to fill the bytes before the 8-byte aligned address)

    // Variables needed to fill the 8-byte aligned memory blocks
    auto remainingBlocks = (length - alignDifference) / 8; // Number of 8-byte blocks to fill
    auto *target = reinterpret_cast<uint64_t*>(address + alignDifference); // Start of the 8-byte aligned memory block
    auto longValue = static_cast<uint64_t>(value); // 8-byte value to fill the memory block
    longValue = longValue | longValue << 8 | longValue << 16 | longValue << 24 | longValue << 32 | longValue << 40 | longValue << 48 | longValue << 56;

    // Variables needed to fill the remaining bytes
    auto remainingBytes = (length - alignDifference) % 8; // Number of remaining bytes to fill
    auto *restuint32_target = reinterpret_cast<uint8_t*>(target) + (remainingBlocks * 8); // Start of the remaining memory block (used to fill the remaining bytes)

    // First fill the bytes up to the next 8-byte aligned address
    while (alignDifference-- > 0) {
        *beforeAlignuint32_target++ = value;
    }

    // Now fill the 8-byte aligned memory blocks
    while (remainingBlocks-- > 0) {
        *target++ = longValue;
    }

    // Finally fill the remaining bytes
    while (remainingBytes-- > 0) {
        *restuint32_target++ = value;
    }
}

void Address::copyRange(const Address &sourceAddress, uint32_t length) const {
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
    auto alignDifference = address % 8; // Number of bytes to next 8-byte aligned address
    auto beforeAlignSource = reinterpret_cast<uint8_t*>(sourceAddress.get()); // Start of the source memory block (used to copy the bytes before the 8-byte aligned address)
    auto beforeAlignuint32_target = reinterpret_cast<uint8_t*>(address); // Start of the target memory block (used to copy the bytes before the 8-byte aligned address)

    // Variables needed to fill the 8-byte aligned memory blocks
    auto remainingBlocks = (length - alignDifference) / 8; // Number of 8-byte blocks to fill
    auto *source = reinterpret_cast<uint64_t*>(sourceAddress.get() + alignDifference); // Start of the 8-byte aligned source memory block
    auto *target = reinterpret_cast<uint64_t*>(address + alignDifference); // Start of the 8-byte aligned target memory block

    // Variables needed to fill the remaining bytes
    auto remainingBytes = (length - alignDifference) % 8; // Number of remaining bytes to fill
    auto *restSource = reinterpret_cast<uint8_t*>(source) + (remainingBlocks * 8); // Start of the remaining source memory block (used to copy the remaining bytes)
    auto *restuint32_target = reinterpret_cast<uint8_t*>(target) + (remainingBlocks * 8); // Start of the remaining target memory block (used to copy the remaining bytes)

    // First fill the bytes up to the next 8-byte aligned address
    while (alignDifference-- > 0) {
        *beforeAlignuint32_target++ = *beforeAlignSource++;
    }

    // Now fill the 8-byte aligned memory blocks
    while (remainingBlocks-- > 0) {
        *target++ = *source++;
    }

    // Finally fill the remaining bytes
    while (remainingBytes-- > 0) {
        *restuint32_target++ = *restSource++;
    }
}

void Address::copyString(const Address &sourceAddress) const {
    auto *target = reinterpret_cast<uint8_t*>(address);
    auto *source = reinterpret_cast<uint8_t*>(sourceAddress.address);

    uint32_t i;
    for (i = 0; source[i] != 0; i++) {
        target[i] = source[i];
    }
    target[i] = 0;
}

void Address::copyString(const Address &sourceAddress, uint32_t maxBytes) const {
    auto *target = reinterpret_cast<uint8_t*>(address);
    auto *source = reinterpret_cast<uint8_t*>(sourceAddress.address);

    uint32_t i;
    for (i = 0; source[i] != 0 && i < maxBytes; i++) {
        target[i] = source[i];
    }
    target[i] = 0;

    for (; i < maxBytes; i++) {
        target[i] = 0;
    }
}

int32_t Address::compareRange(const Address &otherAddress, uint32_t length) const {
    auto *pointer = reinterpret_cast<uint8_t*>(address);
    auto *other = reinterpret_cast<uint8_t*>(otherAddress.address);

    uint32_t i;
    for (i = 0; i < length && pointer[i] == other[i]; i++){}
    return i == length ? 0 : pointer[i] - other[i];
}

int32_t Address::compareString(const Address &otherAddress) const {
    auto *pointer = reinterpret_cast<uint8_t*>(address);
    auto *other = reinterpret_cast<uint8_t*>(otherAddress.address);

    uint32_t i;
    for (i = 0; pointer[i] != 0 && other[i] != 0 && pointer[i] == other[i]; i++){}
    return  pointer[i] - other[i];
}

int32_t Address::compareString(const char *otherString) const {
    return compareString(Address(otherString));
}

Address Address::searchCharacter(uint8_t character) const {
    auto *pointer = reinterpret_cast<uint8_t*>(address);

    uint32_t i;
    for (i = 0; pointer[i] != 0 && pointer[i] != character; i++) {}
    return pointer[i] == 0 ? set(0) : add(i);
}

}