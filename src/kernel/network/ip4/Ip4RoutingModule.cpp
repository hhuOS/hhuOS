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
 * The UDP/IP stack is based on a bachelor's thesis, written by Hannes Feil.
 * The original source code can be found here: https://github.com/hhuOS/hhuOS/tree/legacy/network
 */

#include "Ip4RoutingModule.h"

#include <cstdint>

#include "lib/util/network/ip4/Ip4Address.h"
#include "lib/util/network/ip4/Ip4NetworkMask.h"
#include "kernel/network/ip4/Ip4Route.h"
#include "kernel/network/ip4/Ip4Interface.h"
#include "lib/util/Exception.h"
#include "lib/util/network/NetworkAddress.h"

namespace Kernel::Network::Ip4 {

void Ip4RoutingModule::setDefaultRoute(const Ip4Route &route) {
    defaultRoute = route;
}

void Ip4RoutingModule::addRoute(const Ip4Route &route) {
    if (!routes.contains(route)) {
        routes.add(route);
    }
}

void Ip4RoutingModule::removeRoute(const Ip4Route &route) {
    routes.remove(route);
}

const Ip4Route& Ip4RoutingModule::findRoute(const Util::Network::Ip4::Ip4Address &sourceAddress, const Util::Network::Ip4::Ip4Address &address) const {
    uint8_t longestPrefix = 0;
    const Ip4Route *bestRoute = nullptr;
    bool anySource = sourceAddress == Util::Network::Ip4::Ip4Address::ANY;

    for (const auto &route : routes) {
        if (anySource || sourceAddress == route.getInterface().getAddress()) {
            auto subnetAddress = route.getNetworkMask().extractSubnet(route.getDestinationAddress());
            if (address.compareTo(subnetAddress) > longestPrefix) {
                bestRoute = &route;
            }
        }
    }

    if (bestRoute == nullptr) {
        if (anySource || sourceAddress == defaultRoute.getInterface().getAddress()) {
            return defaultRoute;
        }

        Util::Exception::throwException(Util::Exception::INVALID_ARGUMENT, "Ip4RoutingModule: No route to host!");
    }

    return *bestRoute;
}

}