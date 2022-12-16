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

#include "lib/util/memory/Address.h"

namespace Util::Memory {

template<typename T>
MmxAddress<T>::MmxAddress(T address) : Address<T>(address) {}

template<>
MmxAddress<uint32_t>::MmxAddress(void *pointer) : Address<uint32_t>(pointer) {}

template<>
MmxAddress<uint32_t>::MmxAddress(const void *pointer) : Address<uint32_t>(pointer) {}

template<typename T>
MmxAddress<T>::MmxAddress(const Address<T> &address) : MmxAddress(address.get()) {}

template<typename T>
void MmxAddress<T>::setRange(uint8_t value, T length) const {
    auto *target = reinterpret_cast<uint64_t*>(Address<T>::address);
    auto longValue = static_cast<uint64_t>(value);
    longValue = longValue | longValue << 8 | longValue << 16 | longValue << 24 | longValue << 32 | longValue << 40 | longValue << 48 | longValue << 56;

    asm volatile (
            "movq (%0), %%mm0;"
            "movq %%mm0, %%mm1;"
            "movq %%mm0, %%mm2;"
            "movq %%mm0, %%mm3;"
            "movq %%mm0, %%mm4;"
            "movq %%mm0, %%mm5;"
            "movq %%mm0, %%mm6;"
            "movq %%mm0, %%mm7;"
            : :
            "r"(&longValue)
            );

    while (length - 8 * sizeof(uint64_t) < length) {
        asm volatile (
                "movq %%mm0, (%0);"
                "movq %%mm1, 8(%0);"
                "movq %%mm2, 16(%0);"
                "movq %%mm3, 24(%0);"
                "movq %%mm4, 32(%0);"
                "movq %%mm5, 40(%0);"
                "movq %%mm6, 48(%0);"
                "movq %%mm7, 56(%0);"
                : :
                "r"(target)
                );
        target += 8;
        length -= 8 * sizeof(uint64_t);
    }

    auto *rest = reinterpret_cast<uint8_t*>(target);
    while (length-- > 0) {
        *rest++ = value;
    }
}

template<typename T>
void MmxAddress<T>::copyRange(const Address<T> &sourceAddress, T length) const {
    auto *target = reinterpret_cast<uint64_t*>(Address<T>::address);
    auto *source = reinterpret_cast<uint64_t*>(sourceAddress.get());

    while (length - 8 * sizeof(uint64_t) < length) {
        asm volatile (
                "movq (%0), %%mm0;"
                "movq 8(%0), %%mm1;"
                "movq 16(%0), %%mm2;"
                "movq 24(%0), %%mm3;"
                "movq 32(%0), %%mm4;"
                "movq 40(%0), %%mm5;"
                "movq 48(%0), %%mm6;"
                "movq 56(%0), %%mm7;"
                "movq %%mm0, (%1);"
                "movq %%mm1, 8(%1);"
                "movq %%mm2, 16(%1);"
                "movq %%mm3, 24(%1);"
                "movq %%mm4, 32(%1);"
                "movq %%mm5, 40(%1);"
                "movq %%mm6, 48(%1);"
                "movq %%mm7, 56(%1);"
                : :
                "r"(source),
                "r"(target)
                );
        source += 8;
        target += 8;
        length -= 8 * sizeof(uint64_t);
    }

    auto *targetRest = reinterpret_cast<uint8_t*>(target);
    auto *sourceRest = reinterpret_cast<uint8_t*>(source);
    while (length-- > 0) {
        *targetRest++ = *sourceRest++;
    }
}

}