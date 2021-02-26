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

#include "Address.h"

namespace Util {

template<typename T>
Address<T>::Address() : address(0) {

}

template<typename T>
Address<T>::Address(T address) : address(address) {

}

template<>
Address<uint32_t>::Address(void *pointer) : Address<uint32_t>(reinterpret_cast<uint32_t>(pointer)) {

}

template<>
Address<uint32_t>::Address(const void *pointer) : Address<uint32_t>((void*) pointer) {

}

template<typename T>
bool Address<T>::operator!=(const Address &other) const {
    return address != other.address;
}

template<typename T>
bool Address<T>::operator==(const Address &other) const {
    return address == other.address;
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
Address<T> Address<T>::add(T value) const {
    return Address(address + value);
}

template<typename T>
Address<T> Address<T>::subtract(T value) const {
    return Address<T>(address - value);
}

template<typename T>
Address<T> Address<T>::alignUp(T alignment) const {
    if (alignment == 0) {
        return *this;
    }

    alignment--;

    return Address((address + alignment) & ~alignment);
}

template<typename T>
T Address<T>::stringLength() {
    auto stringAddress = reinterpret_cast<uint8_t*>(address);

    uint32_t i;
    for (i = 0; stringAddress[i] != 0; i++);

    return i;
}

template<typename T>
void Address<T>::setRange(uint8_t value, T amount) {
    auto *destination = reinterpret_cast<uint8_t*>(address);

    for (T i = 0; i < amount; i++) {
        destination[i] = value;
    }
}

template<typename T>
void Address<T>::copyRange(Address<T> sourceAddress, T length) {
    auto *source = reinterpret_cast<uint8_t*>(sourceAddress.get());
    auto *destination = reinterpret_cast<uint8_t*>(address);

    T i;
    for (i = 0; i < length; i++) {
        destination[i] = source[i];
    }
}

template<typename T>
void Address<T>::copyString(Address<T> sourceAddress) {
    auto *source = reinterpret_cast<uint8_t*>(sourceAddress.get());
    auto *destination = reinterpret_cast<uint8_t*>(address);

    T i;
    for (i = 0; source[i] != 0; i++) {
        destination[i] = source[i];
    }
    destination[i] = 0;
}

template<typename T>
void Address<T>::copyString(Address<T> sourceAddress, T maxBytes) {
    auto *source = reinterpret_cast<uint8_t*>(sourceAddress.get());
    auto *destination = reinterpret_cast<uint8_t*>(address);

    T i;
    for (i = 0; source[i] != 0 && i < maxBytes; i++) {
        destination[i] = source[i];
    }

    for (; i < maxBytes; i++) {
        destination[i] = 0;
    }

    destination[i] = 0;
}

template<typename T>
T Address<T>::compareRange(Address<T> otherAddress, T length) {
    auto *current = reinterpret_cast<uint8_t*>(address);
    auto *other = reinterpret_cast<uint8_t*>(otherAddress.get());

    T i;
    for (i = 0; i < length && current[i] == other[i]; i++);

    return i == length ? 0 : current[i] - other[i];
}

template<typename T>
T Address<T>::compareString(Address<T> otherAddress) {
    auto *current = reinterpret_cast<uint8_t*>(address);
    auto *other = reinterpret_cast<uint8_t*>(otherAddress.get());

    T i;
    for (i = 0; current[i] != 0 && other[i] != 0 && current[i] == other[i]; i++);

    return current[i] - other[i];
}

}