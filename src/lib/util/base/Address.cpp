/*
 * Copyright (C) 2018-2023 Heinrich-Heine-Universitaet Duesseldorf,
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

#include "lib/util/hardware/CpuId.h"
#include "Address.h"
#include "SseAddress.h"
#include "MmxAddress.h"

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
    auto *target = reinterpret_cast<uint64_t*>(address);
    auto longValue = static_cast<uint64_t>(value);
    longValue = longValue | longValue << 8 | longValue << 16 | longValue << 24 | longValue << 32 | longValue << 40 | longValue << 48 | longValue << 56;

    while (length - sizeof(uint64_t) < length) {
        *target++ = longValue;
        length -= sizeof(uint64_t);
    }

    auto *rest = reinterpret_cast<uint8_t*>(target);
    while (length-- > 0) {
        *rest++ = value;
    }
}

template<typename T>
void Address<T>::copyRange(const Address<T> &sourceAddress, T length) const {
    auto *target = reinterpret_cast<uint32_t*>(address);
    auto *source = reinterpret_cast<uint32_t*>(sourceAddress.get());

    while (length - 4 * sizeof(uint32_t) < length) {
        asm volatile (
                "mov (%0), %%eax;"
                "mov 4(%0), %%ebx;"
                "mov 8(%0), %%ecx;"
                "mov 12(%0), %%edx;"
                "mov %%eax, (%1);"
                "mov %%ebx, 4(%1);"
                "mov %%ecx, 8(%1);"
                "mov %%edx, 12(%1);"
                : :
                "r"(source),
                "r"(target)
                : "eax", "ebx", "ecx", "edx"
                );
        source += 4;
        target += 4;
        length -= 4 * sizeof(uint32_t);
    }

    auto *targetRest = reinterpret_cast<uint8_t*>(target);
    auto *sourceRest = reinterpret_cast<uint8_t*>(source);
    while (length-- > 0) {
        *targetRest++ = *sourceRest++;
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

template<typename T>
Address<T> *Address<T>::createAcceleratedAddress(T address, bool &useMmx) {
    useMmx = false;
    auto features = Hardware::CpuId::getCpuFeatureBits();

    if ((features & Hardware::CpuId::SSE) != 0) {
        return new SseAddress<T>(address);
    } else if ((features & Hardware::CpuId::MMX) != 0) {
        useMmx = true;
        return new MmxAddress<T>(address);
    }

    return new Address<T>(address);
}

}