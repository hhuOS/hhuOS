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
 *
 * The network stack is based on a bachelor's thesis, written by Hannes Feil.
 * The original source code can be found here: https://github.com/hhuOS/hhuOS/tree/legacy/network
 */

#ifndef HHUOS_IP4ADDRESS_H
#define HHUOS_IP4ADDRESS_H

#include <cstdint>

#include "lib/util/network/NetworkAddress.h"
#include "lib/util/memory/String.h"

namespace Util::Network::Ip4 {

class Ip4Address : public NetworkAddress {

public:
    /**
     * Constructor.
     */
    Ip4Address();

    /**
     * Constructor.
     */
    explicit Ip4Address(uint8_t *buffer);

    /**
     * Constructor.
     */
    explicit Ip4Address(const Util::Memory::String &string);

    /**
     * Copy Constructor.
     */
    Ip4Address(const Ip4Address &other) = default;

    /**
     * Assignment operator.
     */
    Ip4Address &operator=(const Ip4Address &other) = default;

    /**
     * Destructor.
     */
    ~Ip4Address() override = default;

    [[nodiscard]] static Ip4Address createBroadcastAddress();

    [[nodiscard]] bool isBroadcastAddress() const;

    [[nodiscard]] NetworkAddress* createCopy() const override;

    void setAddress(const Util::Memory::String &string) override;

    [[nodiscard]] Util::Memory::String toString() const override;

    static const Ip4Address ANY;

    static const constexpr uint8_t ADDRESS_LENGTH = 4;
};

}

#endif
