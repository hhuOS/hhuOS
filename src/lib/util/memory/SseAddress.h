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

#ifndef HHUOS_SSEADDRESS_H
#define HHUOS_SSEADDRESS_H

#include <cstdint>

#include "Address.h"

namespace Util::Memory {

template<typename T>
class SseAddress : public Address<T> {

public:
    /**
     * Default Constructor.
     */
    SseAddress() = default;

    explicit SseAddress(T address);

    explicit SseAddress(void *pointer);

    explicit SseAddress(const void *pointer);

    explicit SseAddress(const Address<T> &address);

    /**
     * Copy Constructor.
     */
    SseAddress(const SseAddress &other) = delete;

    /**
     * Assignment operator.
     */
    SseAddress &operator=(const SseAddress &other) = delete;

    /**
     * Destructor.
     */
    ~SseAddress() override = default;

    void setRange(uint8_t value, T length) const override;

    void copyRange(const Address<T> &sourceAddress, T length) const override;
};

template
class SseAddress<uint16_t>;

template
class SseAddress<uint32_t>;

}

#endif
