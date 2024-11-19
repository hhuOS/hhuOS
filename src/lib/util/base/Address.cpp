/*
 * Copyright (C) 2018-2024 Heinrich-Heine-Universitaet Duesseldorf,
 * Institute of Computer Science, Department Operating Systems
 * Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schoettner
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

template<typename T>
Address<T>::Address(T address) : address(address) {}

template<>
Address<uint32_t>::Address(void *pointer) : address(reinterpret_cast<uint32_t>(pointer)) {}

template<>
Address<uint32_t>::Address(const void *pointer) : address(reinterpret_cast<uint32_t>(pointer)) {}

template<typename T>
bool Address<T>::operator==(const Address &other) const {
    return address == other.address;
}

template<typename T>
bool Address<T>::operator!=(const Address &other) const {
    return address != other.address;
}

template<typename T>
bool Address<T>::operator==(T otherAddress) const {
    return address == otherAddress;
}

template<typename T>
bool Address<T>::operator!=(T otherAddress) const {
    return address != otherAddress;
}

template<typename T>
Address<T>::operator T() const {
    return address;
}

template<typename T>
T Address<T>::get() const {
    return address;
}

template<typename T>
Address<T> Address<T>::set(T newAddress) const {
    return Address<T>(newAddress);
}

template<typename T>
Address<T> Address<T>::add(T value) const {
    return set(address + value);
}

template<typename T>
Address<T> Address<T>::subtract(T value) const {
    return set(address - value);
}

template<typename T>
Address<T> Address<T>::alignUp(T alignment) const {
    if (alignment == 0) {
        return set(address);
    }

    alignment--;
    return set((address + alignment) & ~alignment);
}

template<typename T>
uint8_t Address<T>::getByte(T offset) const {
    return *reinterpret_cast<uint8_t*>(address + offset);
}

template<typename T>
uint16_t Address<T>::getShort(T offset) const {
    return *reinterpret_cast<uint16_t*>(address + offset);
}

template<typename T>
uint32_t Address<T>::getInt(T offset) const {
    return *reinterpret_cast<uint32_t*>(address + offset);
}

template<typename T>
uint64_t Address<T>::getLong(T offset) const {
    return *reinterpret_cast<uint64_t*>(address + offset);
}

template<typename T>
void Address<T>::setByte(uint8_t value, T offset) const {
    *reinterpret_cast<uint8_t*>(address + offset) = value;
}

template<typename T>
void Address<T>::setShort(uint16_t value, T offset) const {
   *reinterpret_cast<uint16_t*>(address + offset) = value;
}

template<typename T>
void Address<T>::setInt(uint32_t value, T offset) const {
    *reinterpret_cast<uint32_t*>(address + offset) = value;
}

template<typename T>
void Address<T>::setLong(uint64_t value, T offset) const {
    *reinterpret_cast<uint64_t*>(address + offset) = value;
}

template<typename T>
T Address<T>::stringLength() const {
    auto *pointer = reinterpret_cast<uint8_t*>(address);

    T i;
    for (i = 0; pointer[i] != 0; i++) {}

    return i;
}

template<typename T>
void Address<T>::setRange(uint8_t value, T length) const {
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
    auto beforeAlignTarget = reinterpret_cast<uint8_t*>(address); // Start of the memory block (used to fill the bytes before the 8-byte aligned address)

    // Variables needed to fill the 8-byte aligned memory blocks
    auto remainingBlocks = (length - alignDifference) / 8; // Number of 8-byte blocks to fill
    auto *target = reinterpret_cast<uint64_t*>(address + alignDifference); // Start of the 8-byte aligned memory block
    auto longValue = static_cast<uint64_t>(value); // 8-byte value to fill the memory block
    longValue = longValue | longValue << 8 | longValue << 16 | longValue << 24 | longValue << 32 | longValue << 40 | longValue << 48 | longValue << 56;

    // Variables needed to fill the remaining bytes
    auto remainingBytes = (length - alignDifference) % 8; // Number of remaining bytes to fill
    auto *restTarget = reinterpret_cast<uint8_t*>(target) + (remainingBlocks * 8); // Start of the remaining memory block (used to fill the remaining bytes)

    // First fill the bytes up to the next 8-byte aligned address
    while (alignDifference-- > 0) {
        *beforeAlignTarget++ = value;
    }

    // Now fill the 8-byte aligned memory blocks
    while (remainingBlocks-- > 0) {
        *target++ = longValue;
    }

    // Finally fill the remaining bytes
    while (remainingBytes-- > 0) {
        *restTarget++ = value;
    }
}

template<typename T>
void Address<T>::copyRange(const Address<T> &sourceAddress, T length) const {
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
    auto beforeAlignTarget = reinterpret_cast<uint8_t*>(address); // Start of the target memory block (used to copy the bytes before the 8-byte aligned address)

    // Variables needed to fill the 8-byte aligned memory blocks
    auto remainingBlocks = (length - alignDifference) / 8; // Number of 8-byte blocks to fill
    auto *source = reinterpret_cast<uint64_t*>(sourceAddress.get() + alignDifference); // Start of the 8-byte aligned source memory block
    auto *target = reinterpret_cast<uint64_t*>(address + alignDifference); // Start of the 8-byte aligned target memory block

    // Variables needed to fill the remaining bytes
    auto remainingBytes = (length - alignDifference) % 8; // Number of remaining bytes to fill
    auto *restSource = reinterpret_cast<uint8_t*>(source) + (remainingBlocks * 8); // Start of the remaining source memory block (used to copy the remaining bytes)
    auto *restTarget = reinterpret_cast<uint8_t*>(target) + (remainingBlocks * 8); // Start of the remaining target memory block (used to copy the remaining bytes)

    // First fill the bytes up to the next 8-byte aligned address
    while (alignDifference-- > 0) {
        *beforeAlignTarget++ = *beforeAlignSource++;
    }

    // Now fill the 8-byte aligned memory blocks
    while (remainingBlocks-- > 0) {
        *target++ = *source++;
    }

    // Finally fill the remaining bytes
    while (remainingBytes-- > 0) {
        *restTarget++ = *restSource++;
    }
}

template<typename T>
void Address<T>::copyString(const Address<T> &sourceAddress) const {
    auto *target = reinterpret_cast<uint8_t*>(address);
    auto *source = reinterpret_cast<uint8_t*>(sourceAddress.address);

    T i;
    for (i = 0; source[i] != 0; i++) {
        target[i] = source[i];
    }
    target[i] = 0;
}

template<typename T>
void Address<T>::copyString(const Address<T> &sourceAddress, T maxBytes) const {
    auto *target = reinterpret_cast<uint8_t*>(address);
    auto *source = reinterpret_cast<uint8_t*>(sourceAddress.address);

    T i;
    for (i = 0; source[i] != 0 && i < maxBytes; i++) {
        target[i] = source[i];
    }
    target[i] = 0;

    for (; i < maxBytes; i++) {
        target[i] = 0;
    }
}

template<typename T>
int32_t Address<T>::compareRange(const Address<T> &otherAddress, T length) const {
    auto *pointer = reinterpret_cast<uint8_t*>(address);
    auto *other = reinterpret_cast<uint8_t*>(otherAddress.address);

    T i;
    for (i = 0; i < length && pointer[i] == other[i]; i++){}
    return i == length ? 0 : pointer[i] - other[i];
}

template<typename T>
int32_t Address<T>::compareString(const Address<T> &otherAddress) const {
    auto *pointer = reinterpret_cast<uint8_t*>(address);
    auto *other = reinterpret_cast<uint8_t*>(otherAddress.address);

    T i;
    for (i = 0; pointer[i] != 0 && other[i] != 0 && pointer[i] == other[i]; i++){}
    return  pointer[i] - other[i];
}

template<>
int32_t Address<uint32_t>::compareString(const char *otherString) const {
    return compareString(Address(otherString));
}

template<typename T>
Address<T> Address<T>::searchCharacter(uint8_t character) const {
    auto *pointer = reinterpret_cast<uint8_t*>(address);

    T i;
    for (i = 0; pointer[i] != 0 && pointer[i] != character; i++) {}
    return pointer[i] == 0 ? set(0) : add(i);
}

}