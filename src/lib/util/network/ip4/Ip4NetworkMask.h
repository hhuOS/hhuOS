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

#ifndef HHUOS_IP4NETWORKMASK_H
#define HHUOS_IP4NETWORKMASK_H

#include <cstdint>

#include "lib/util/network/NetworkMask.h"
#include "lib/util/network/ip4/Ip4Address.h"

namespace Util::Network::Ip4 {

class Ip4NetworkMask : public NetworkMask {

public:
    /**
     * Default Constructor.
     */
    explicit Ip4NetworkMask();

    /**
     * Constructor.
     */
    explicit Ip4NetworkMask(uint8_t bitCount);

    /**
     * Copy Constructor.
     */
    Ip4NetworkMask(const Ip4NetworkMask &other) = default;

    /**
     * Assignment operator.
     */
    Ip4NetworkMask &operator=(const Ip4NetworkMask &other) = default;

    /**
     * Destructor.
     */
    ~Ip4NetworkMask() = default;

    bool operator==(const Ip4NetworkMask &other) const;

    bool operator!=(const Ip4NetworkMask &other) const;

    [[nodiscard]] Util::Network::Ip4::Ip4Address extractSubnet(const Util::Network::Ip4::Ip4Address &address) const;

    [[nodiscard]] Util::Network::Ip4::Ip4Address createBroadcastAddress(const Util::Network::Ip4::Ip4Address &networkAddress) const;
};

}

#endif
