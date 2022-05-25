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

#include "MmxAddress.h"
#include "lib/util/cpu/CpuId.h"

namespace Util::Memory {

template<typename T>
bool MmxAddress<T>::mmxAvailable = Cpu::CpuId::isAvailable() && (Cpu::CpuId::getCpuFeatureBits() & Cpu::CpuId::MMX) != 0;

template<typename T>
MmxAddress<T>::MmxAddress(T address) : Address<T>(address) {}

template<>
MmxAddress<uint32_t>::MmxAddress(void *pointer) : Address<uint32_t>(pointer) {}

template<>
MmxAddress<uint32_t>::MmxAddress(const void *pointer) : Address<uint32_t>(pointer) {}

template<typename T>
bool MmxAddress<T>::isAvailable() {
    return mmxAvailable;
}

template<typename T>
void MmxAddress<T>::setRange(uint8_t value, T length) const {
    auto *target = reinterpret_cast<uint64_t*>(Address<T>::address);
    auto longValue = static_cast<uint64_t>(value);
    longValue = longValue | longValue << 8 | longValue << 16 | longValue << 24 | longValue << 32 | longValue << 40 | longValue << 48 | longValue << 56;

    asm volatile (
            "movq (%0),%%mm0;"
            : :
            "r"(&longValue)
            );

    while (length - sizeof(uint64_t) < length) {
        asm volatile (
                "movq %%mm0,(%0);"
                : :
                "r"(target++)
                );
        length -= sizeof(uint64_t);
    }

    auto *rest = reinterpret_cast<uint8_t*>(target);
    while (length-- > 0) {
        *rest++ = value;
    }
}

template<typename T>
void MmxAddress<T>::copyRange(Address<T> sourceAddress, T length) const {
    auto *target = reinterpret_cast<uint64_t*>(Address<T>::address);
    auto *source = reinterpret_cast<uint64_t*>(sourceAddress.get());

    while (length - sizeof(uint64_t) < length) {
        asm volatile (
                "movq (%0),%%mm0;"
                "movq %%mm0,(%1);"
                : :
                "r"(source++),
                "r"(target++)
                );
        length -= sizeof(uint64_t);
    }

    auto *targetRest = reinterpret_cast<uint8_t*>(target);
    auto *sourceRest = reinterpret_cast<uint8_t*>(source);
    while (length-- > 0) {
        *targetRest++ = *sourceRest++;
    }
}

template<typename T>
void MmxAddress<T>::endMmxOperation() {
    asm volatile ("emms");
}

}