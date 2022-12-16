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

#include "SseAddress.h"

#include "lib/util/memory/Address.h"

namespace Util::Memory {

template<typename T>
SseAddress<T>::SseAddress(T address) : Address<T>(address) {}

template<>
SseAddress<uint32_t>::SseAddress(void *pointer) : Address<uint32_t>(pointer) {}

template<>
SseAddress<uint32_t>::SseAddress(const void *pointer) : Address<uint32_t>(pointer) {}

template<typename T>
SseAddress<T>::SseAddress(const Address<T> &address) : SseAddress(address.get()) {}

template<typename T>
void SseAddress<T>::setRange(uint8_t value, T length) const {
    auto *target = reinterpret_cast<uint64_t *>(Address<T>::address);
    auto longValue = static_cast<uint64_t>(value);
    longValue = longValue | longValue << 8 | longValue << 16 | longValue << 24 | longValue << 32 | longValue << 40 | longValue << 48 | longValue << 56;
    uint64_t longArray[]{longValue, longValue};

    asm volatile (
            "movdqu (%0),%%xmm0;"
            "movdqu %%xmm0, %%xmm1;"
            "movdqu %%xmm0, %%xmm2;"
            "movdqu %%xmm0, %%xmm3;"
            "movdqu %%xmm0, %%xmm4;"
            "movdqu %%xmm0, %%xmm5;"
            "movdqu %%xmm0, %%xmm6;"
            "movdqu %%xmm0, %%xmm7;"
            : :
            "r"(longArray)
            );

    while (length - 16 * sizeof(uint64_t) < length) {
        asm volatile (
                "movdqu %%xmm0, (%0);"
                "movdqu %%xmm1, 16(%0);"
                "movdqu %%xmm2, 32(%0);"
                "movdqu %%xmm3, 48(%0);"
                "movdqu %%xmm4, 64(%0);"
                "movdqu %%xmm5, 80(%0);"
                "movdqu %%xmm6, 96(%0);"
                "movdqu %%xmm7, 112(%0);"
                : :
                "r"(target)
                );
        target += 16;
        length -= 16 * sizeof(uint64_t);
    }

    auto *rest = reinterpret_cast<uint8_t *>(target);
    while (length-- > 0) {
        *rest++ = value;
    }
}

template<typename T>
void SseAddress<T>::copyRange(const Address<T> &sourceAddress, T length) const {
    auto *target = reinterpret_cast<uint64_t *>(Address<T>::address);
    auto *source = reinterpret_cast<uint64_t *>(sourceAddress.get());

    while (length - 16 * sizeof(uint64_t) < length) {
        asm volatile (
                "movdqu (%0),%%xmm0;"
                "movdqu 16(%0),%%xmm1;"
                "movdqu 32(%0),%%xmm2;"
                "movdqu 48(%0),%%xmm3;"
                "movdqu 64(%0),%%xmm4;"
                "movdqu 80(%0),%%xmm5;"
                "movdqu 96(%0),%%xmm6;"
                "movdqu 112(%0),%%xmm7;"
                "movdqu %%xmm0,(%1);"
                "movdqu %%xmm1,16(%1);"
                "movdqu %%xmm2,32(%1);"
                "movdqu %%xmm3,48(%1);"
                "movdqu %%xmm4,64(%1);"
                "movdqu %%xmm5,80(%1);"
                "movdqu %%xmm6,96(%1);"
                "movdqu %%xmm7,112(%1);"
                : :
                "r"(source),
                "r"(target)
                );
        source += 16;
        target += 16;
        length -= 16 * sizeof(uint64_t);
    }

    auto *targetRest = reinterpret_cast<uint8_t *>(target);
    auto *sourceRest = reinterpret_cast<uint8_t *>(source);
    while (length-- > 0) {
        *targetRest++ = *sourceRest++;
    }
}

}