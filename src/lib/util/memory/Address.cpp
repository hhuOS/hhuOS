/*
 * Copyright (C) 2018-2021 Heinrich-Heine-Universitaet Duesseldorf,
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
Address<T>::Address(T address, T limitOffset) : address(address), limit(address + limitOffset < address ? static_cast<T>(0xffffffff) : address + limitOffset) {}

template<>
Address<uint32_t>::Address(void *pointer, uint32_t limitOffset) : Address<uint32_t>(reinterpret_cast<uint32_t>(pointer), limitOffset) {}

template<>
Address<uint32_t>::Address(const void *pointer, uint32_t limitOffset) : Address<uint32_t>((void*) pointer, limitOffset) {}

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
T Address<T>::getLimit() const {
    return limit;
}

template<typename T>
Address<T> Address<T>::set(T newAddress) const {
    if (newAddress > limit) {
        Exception::throwException(Exception::OUT_OF_BOUNDS, "Address: Trying to initialize new address above limit!");
    }

    auto ret = Address<T>(newAddress);
    ret.limit = limit;

    return ret;
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
        return *this;
    }

    alignment--;

    return set((address + alignment) & ~alignment);
}

template<typename T>
uint8_t Address<T>::getByte(T offset) const {
    if (address + offset > limit) {
        Exception::throwException(Exception::OUT_OF_BOUNDS, "Address: Read access above limit!");
    }

    return *reinterpret_cast<uint8_t*>(address + offset);
}

template<typename T>
uint16_t Address<T>::getShort(T offset) const {
    if (address + offset + sizeof(uint16_t) - 1 > limit) {
        Exception::throwException(Exception::OUT_OF_BOUNDS, "Address: Write access above limit!");
    }

    return *reinterpret_cast<uint16_t*>(address + offset);
}

template<typename T>
uint32_t Address<T>::getInt(T offset) const {
    if (address + offset + sizeof(uint32_t) - 1 > limit) {
        Exception::throwException(Exception::OUT_OF_BOUNDS, "Address: Write access above limit!");
    }

    return *reinterpret_cast<uint32_t*>(address + offset);
}

template<typename T>
uint64_t Address<T>::getLong(T offset) const {
    if (address + offset + sizeof(uint64_t) - 1 > limit) {
        Exception::throwException(Exception::OUT_OF_BOUNDS, "Address: Write access above limit!");
    }

    return *reinterpret_cast<uint64_t*>(address + offset);
}

template<typename T>
void Address<T>::setByte(uint8_t value, T offset) const {
    if (address + offset > limit) {
        Exception::throwException(Exception::OUT_OF_BOUNDS, "Address: Write access above limit!");
    }

    *reinterpret_cast<uint8_t*>(address + offset) = value;
}

template<typename T>
void Address<T>::setShort(uint16_t value, T offset) const {
    if (address + offset + sizeof(uint16_t) > limit) {
        Exception::throwException(Exception::OUT_OF_BOUNDS, "Address: Write access above limit!");
    }

   *reinterpret_cast<uint16_t*>(address + offset) = value;
}

template<typename T>
void Address<T>::setInt(uint32_t value, T offset) const {
    if (address + offset + sizeof(uint32_t) > limit) {
        Exception::throwException(Exception::OUT_OF_BOUNDS, "Address: Write access above limit!");
    }

    *reinterpret_cast<uint32_t*>(address + offset) = value;
}

template<typename T>
void Address<T>::setLong(uint64_t value, T offset) const {
    if (address + offset + sizeof(uint64_t) > limit) {
        Exception::throwException(Exception::OUT_OF_BOUNDS, "Address: Write access above limit!");
    }

    *reinterpret_cast<uint64_t*>(address + offset) = value;
}

template<typename T>
T Address<T>::stringLength() const {
    uint32_t i;
    for (i = 0; getByte(i) != 0; i++);

    return i;
}

template<typename T>
void Address<T>::setRange(uint8_t value, T length) const {
    auto longValue = static_cast<uint64_t>(value);
    longValue = longValue | longValue << 8 | longValue << 16 | longValue << 24 | longValue << 32 | longValue << 40 | longValue << 48 | longValue << 56;

    T i;
    for (i = 0; i + sizeof(uint64_t) <= length; i += sizeof(uint64_t)) {
        setLong(longValue, i);
    }

    for (; i < length; i++) {
        setByte(value, i);
    }
}

template<typename T>
void Address<T>::copyRange(Address<T> sourceAddress, T length) const {
    T i;
    for (i = 0; i + sizeof(uint64_t) <= length; i += sizeof(uint64_t)) {
        setLong(sourceAddress.getLong(i), i);
    }

    for (; i < length; i++) {
        setByte(sourceAddress.getByte(i), i);
    }
}

template<typename T>
void Address<T>::copyString(Address<T> sourceAddress) const {
    T i;
    for (i = 0; sourceAddress.getByte(i) != 0; i++) {
        setByte(sourceAddress.getByte(i), i);
    }
    setByte(0, i);
}

template<typename T>
void Address<T>::copyString(Address<T> sourceAddress, T maxBytes) const {
    T i;
    for (i = 0; sourceAddress.getByte(i) != 0 && i < maxBytes; i++) {
        setByte(sourceAddress.getByte(i), i);
    }

    for (; i < maxBytes; i++) {
        setByte(0, i);
    }
}

template<typename T>
T Address<T>::compareRange(Address<T> otherAddress, T length) const {
    T i;
    for (i = 0; i < length && getByte(i) == otherAddress.getByte(i); i++);

    return i == length ? 0 : getByte(i) - otherAddress.getByte(i);
}

template<typename T>
T Address<T>::compareString(Address<T> otherAddress) const {
    T i;
    for (i = 0; getByte(i) != 0 && otherAddress.getByte(i) != 0 && getByte(i) == otherAddress.getByte(i); i++);

    return getByte(i) - otherAddress.getByte(i);
}

}