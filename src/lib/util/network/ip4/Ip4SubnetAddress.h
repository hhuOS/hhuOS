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

#ifndef HHUOS_IP4SUBNETADDRESS_H
#define HHUOS_IP4SUBNETADDRESS_H

#include <cstdint>

#include "lib/util/network/NetworkAddress.h"
#include "Ip4Address.h"
#include "lib/util/base/String.h"

namespace Util::Network::Ip4 {

class Ip4SubnetAddress : public NetworkAddress {

public:
    /**
     * Default Constructor.
     */
    Ip4SubnetAddress();

    /**
     * Constructor.
     */
    explicit Ip4SubnetAddress(uint8_t *buffer);

    /**
     * Constructor.
     */
    explicit Ip4SubnetAddress(const Util::String &string);

    /**
     * Constructor.
     */
    Ip4SubnetAddress(const Ip4Address &address, uint8_t bitCount);

    /**
     * Constructor.
     */
    explicit Ip4SubnetAddress(const Ip4Address &address);

    /**
     * Constructor.
     */
    explicit Ip4SubnetAddress(uint8_t bitCount);

    /**
     * Copy Constructor.
     */
    Ip4SubnetAddress(const Ip4SubnetAddress &other) = default;

    /**
     * Assignment operator.
     */
    Ip4SubnetAddress &operator=(const Ip4SubnetAddress &other) = default;

    /**
     * Destructor.
     */
    ~Ip4SubnetAddress() override = default;

    [[nodiscard]] Ip4Address getIp4Address() const;

    [[nodiscard]] uint8_t getBitCount() const;

    [[nodiscard]] Ip4SubnetAddress getSubnetAddress() const;

    [[nodiscard]] Ip4Address getBroadcastAddress() const;

    [[nodiscard]] NetworkAddress* createCopy() const override;

    [[nodiscard]] Util::String toString() const override;

    static const uint32_t ADDRESS_LENGTH = Ip4Address::ADDRESS_LENGTH + sizeof(uint8_t);
};

}

#endif
