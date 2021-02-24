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

}