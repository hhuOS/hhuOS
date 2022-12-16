/*
 * Copyright (C) 2018-2022 Heinrich-Heine-Universitaet Duesseldorf,
 * Institute of Computer Science, Department Operating Systems
 * Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Hannes Feil, Michael Schoettner
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

#ifndef HHUOS_IP4ROUTE_H
#define HHUOS_IP4ROUTE_H

#include "Ip4Address.h"
#include "Ip4NetworkMask.h"
#include "lib/util/memory/String.h"

namespace Network {
namespace Ip4 {
class Ip4Interface;
}  // namespace Ip4
}  // namespace Network

namespace Network::Ip4 {

class Ip4Route {

public:
    /**
     * Default Constructor.
     */
    Ip4Route() = default;

    /**
     * Constructor.
     */
    Ip4Route(const Ip4Address &localAddress, const Ip4NetworkMask &networkMask, const Ip4Address &nextHop, const Util::Memory::String &device);

    /**
     * Constructor.
     */
    Ip4Route(const Ip4Address &localAddress, const Ip4NetworkMask &networkMask, const Util::Memory::String &device);

    /**
     * Copy Constructor.
     */
    Ip4Route(const Ip4Route &other) = default;

    /**
     * Assignment operator.
     */
    Ip4Route &operator=(const Ip4Route &other) = default;

    /**
     * Destructor.
     */
    ~Ip4Route() = default;

    bool operator==(const Ip4Route &other) const;

    bool operator!=(const Ip4Route &other) const;

    [[nodiscard]] const Ip4Address& getAddress() const;

    [[nodiscard]] const Ip4NetworkMask& getNetworkMask() const;

    [[nodiscard]] const Ip4Interface& getInterface() const;

    [[nodiscard]] bool hasNextHop() const;

    [[nodiscard]] const Ip4Address& getNextHop() const;

private:

    Ip4Address address{};
    Ip4NetworkMask networkMask{};
    Ip4Address nextHop{};
    Ip4Interface *interface{};

    bool nextHopValid{};
};

}

#endif
