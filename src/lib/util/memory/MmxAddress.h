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

#ifndef HHUOS_MMXADDRESS_H
#define HHUOS_MMXADDRESS_H

#include <cstdint>

#include "Address.h"

namespace Util::Memory {

template<typename T>
class MmxAddress : public Address<T> {

public:
    /**
     * Default Constructor.
     */
    MmxAddress() = default;

    explicit MmxAddress(T address);

    explicit MmxAddress(void *pointer);

    explicit MmxAddress(const void *pointer);

    explicit MmxAddress(const Address<T> &address);

    /**
     * Copy Constructor.
     */
    MmxAddress(const MmxAddress &other) = delete;

    /**
     * Assignment operator.
     */
    MmxAddress &operator=(const MmxAddress &other) = delete;

    /**
     * Destructor.
     */
    ~MmxAddress() override = default;

    void setRange(uint8_t value, T length) const override;

    void copyRange(const Address<T> &sourceAddress, T length) const override;
};

template
class MmxAddress<uint16_t>;

template
class MmxAddress<uint32_t>;

}

#endif
