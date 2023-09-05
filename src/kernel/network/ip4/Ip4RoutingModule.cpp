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
 *
 * The UDP/IP stack is based on a bachelor's thesis, written by Hannes Feil.
 * The original source code can be found here: https://github.com/hhuOS/hhuOS/tree/legacy/network
 */

#include <cstdint>

#include "Ip4RoutingModule.h"
#include "lib/util/network/ip4/Ip4Address.h"
#include "lib/util/base/Exception.h"
#include "lib/util/network/NetworkAddress.h"
#include "kernel/system/System.h"
#include "kernel/service/NetworkService.h"
#include "kernel/network/NetworkStack.h"
#include "kernel/network/ip4/Ip4Module.h"
#include "lib/util/network/ip4/Ip4SubnetAddress.h"
#include "kernel/network/ip4/Ip4Interface.h"
#include "lib/util/collection/Iterator.h"

namespace Kernel::Network::Ip4 {

bool Ip4RoutingModule::addRoute(const Util::Network::Ip4::Ip4Route &route) {
    auto &ip4Module = System::getService<NetworkService>().getNetworkStack().getIp4Module();
    if (ip4Module.getTargetInterfaces(route.getSourceAddress()).length() == 0) {
        return false;
    }

    bool ret = false;
    lock.acquire();

    if (route.getAddress().getBitCount() == 0) {
        routes.remove(route);
        defaultRoute = route;
        ret = true;
    } else if (!routes.contains(route)) {
        ret = routes.add(route);
    }

    lock.release();
    return ret;
}

bool Ip4RoutingModule::removeRoute(const Util::Network::Ip4::Ip4Route &route) {
    bool ret;
    lock.acquire();

    if (route == defaultRoute) {
        defaultRoute = Util::Network::Ip4::Ip4Route();
        ret = true;
    } else {
        ret = routes.remove(route);
    }

    lock.release();
    return ret;
}

void Ip4RoutingModule::removeRoute(const Util::Network::Ip4::Ip4SubnetAddress &localAddress, const Util::String &device) {
    auto route = Util::Network::Ip4::Ip4Route(localAddress, device);
    removeRoute(route);
}

const Util::Network::Ip4::Ip4Route &Ip4RoutingModule::getDefaultRoute() const {
    return defaultRoute;
}

Util::Array<Util::Network::Ip4::Ip4Route> Ip4RoutingModule::getRoutes(const Util::Network::Ip4::Ip4Address &sourceAddress) {
    auto ret = Util::ArrayList<Util::Network::Ip4::Ip4Route>();
    bool anySource = sourceAddress == Util::Network::Ip4::Ip4Address::ANY;

    lock.acquire();

    for (const auto &route : routes) {
        if (anySource || route.getSourceAddress() == sourceAddress) {
            ret.add(route);
        }
    }

    if (anySource || defaultRoute.getSourceAddress() == sourceAddress) {
        ret.add(defaultRoute);
    }
    lock.release();

    return ret.toArray();
}

const Util::Network::Ip4::Ip4Route& Ip4RoutingModule::findRoute(const Util::Network::Ip4::Ip4Address &sourceAddress, const Util::Network::Ip4::Ip4Address &address) {
    uint8_t longestPrefix = 0;
    const Util::Network::Ip4::Ip4Route *bestRoute = nullptr;
    bool anySource = sourceAddress == Util::Network::Ip4::Ip4Address::ANY;

    lock.acquire();
    for (const auto &route : routes) {
        auto routeAddress = route.getSourceAddress();

        if (anySource || sourceAddress == route.getSourceAddress()) {
            auto subnetAddress = route.getTargetAddress();
            auto prefix = address.compareTo(subnetAddress);

            if (prefix >= subnetAddress.getBitCount() && prefix > longestPrefix) {
                bestRoute = &route;
            }
        }
    }
    lock.release();

    if (bestRoute == nullptr) {
        if (defaultRoute.isValid() && (anySource || sourceAddress == defaultRoute.getSourceAddress())) {
            return defaultRoute;
        }

        Util::Exception::throwException(Util::Exception::INVALID_ARGUMENT, "Ip4RoutingModule: No route to host!");
    }

    return *bestRoute;
}

}