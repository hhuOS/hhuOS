/*
 * Copyright (C) 2018-2022 Heinrich-Heine-Universitaet Duesseldorf,
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

#include "lib/util/Exception.h"
#include "Address.h"

namespace Util::Memory {

template<typename T>
Address<T>::Address() : address(0) {}

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
    auto *pointer = reinterpret_cast<uint64_t*>(address);
    auto longValue = static_cast<uint64_t>(value);
    longValue = longValue | longValue << 8 | longValue << 16 | longValue << 24 | longValue << 32 | longValue << 40 | longValue << 48 | longValue << 56;

    T i;
    for (i = 0; i < length / sizeof(uint64_t); i++) {
        pointer[i] = longValue;
    }

    i *= sizeof(uint64_t);
    for (; i < length; i++) {
        reinterpret_cast<uint8_t*>(pointer)[i] = value;
    }
}

template<typename T>
void Address<T>::copyRange(Address<T> sourceAddress, T length) const {
    auto *target = reinterpret_cast<uint64_t*>(address);
    auto *source = reinterpret_cast<uint64_t*>(sourceAddress.get());

    T i;
    for (i = 0; i < length / sizeof(uint64_t); i++) {
        target[i] = source[i];
    }

    i *= sizeof(uint64_t);
    for (; i < length; i++) {
        reinterpret_cast<uint8_t*>(target)[i] = reinterpret_cast<uint8_t*>(source)[i];
    }
}

template<typename T>
void Address<T>::copyString(Address<T> sourceAddress) const {
    auto *target = reinterpret_cast<uint8_t*>(address);
    auto *source = reinterpret_cast<uint8_t*>(sourceAddress.address);

    T i;
    for (i = 0; source[i] != 0; i++) {
        target[i] = source[i];
    }
    target[i] = 0;
}

template<typename T>
void Address<T>::copyString(Address<T> sourceAddress, T maxBytes) const {
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
T Address<T>::compareRange(Address<T> otherAddress, T length) const {
    auto *pointer = reinterpret_cast<uint8_t*>(address);
    auto *other = reinterpret_cast<uint8_t*>(otherAddress.address);

    T i;
    for (i = 0; i < length && pointer[i] == other[i]; i++);
    return i == length ? 0 : pointer[i] - other[i];
}

template<typename T>
T Address<T>::compareString(Address<T> otherAddress) const {
    auto *pointer = reinterpret_cast<uint8_t*>(address);
    auto *other = reinterpret_cast<uint8_t*>(otherAddress.address);

    T i;
    for (i = 0; pointer[i] != 0 && other[i] != 0 && pointer[i] == other[i]; i++);
    return  pointer[i] - other[i];
}

template<>
uint32_t Address<uint32_t>::compareString(const char *otherString) const {
    return compareString(Address<uint32_t>(otherString));
}

}