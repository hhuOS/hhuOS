/*
 * Copyright (C) 2017-2025 Heinrich Heine University Düsseldorf,
 * Institute of Computer Science, Department Operating Systems
 * Main developers: Christian Gesse <christian.gesse@hhu.de>, Fabian Ruhland <ruhland@hhu.de>
 * Original development team: Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schöttner
 * This project has been supported by several students.
 * A full list of integrated student theses can be found here: https://github.com/hhuOS/hhuOS/wiki/Student-theses
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

#ifndef HHUOS_IP4ROUTINGMODULE_H
#define HHUOS_IP4ROUTINGMODULE_H

#include "lib/util/collection/ArrayList.h"
#include "lib/util/collection/Array.h"
#include "lib/util/async/ReentrantSpinlock.h"
#include "lib/util/base/String.h"
#include "lib/util/network/ip4/Ip4Route.h"

namespace Util {
namespace Network {
namespace Ip4 {
class Ip4Address;
class Ip4SubnetAddress;
}  // namespace Ip4
}  // namespace Network
}  // namespace Util

namespace Kernel {
namespace Network {
namespace Ip4 {

class Ip4RoutingModule {

public:
    /**
     * Default Constructor.
     */
    Ip4RoutingModule() = default;

    /**
     * Copy Constructor.
     */
    Ip4RoutingModule(const Ip4RoutingModule &other) = delete;

    /**
     * Assignment operator.
     */
    Ip4RoutingModule &operator=(const Ip4RoutingModule &other) = delete;

    /**
     * Destructor.
     */
    ~Ip4RoutingModule() = default;

    bool addRoute(const Util::Network::Ip4::Ip4Route &route);

    bool removeRoute(const Util::Network::Ip4::Ip4Route &route);

    void removeRoute(const Util::Network::Ip4::Ip4SubnetAddress &localAddress, const Util::String &device);

    const Util::Network::Ip4::Ip4Route& getDefaultRoute() const;

    Util::Array<Util::Network::Ip4::Ip4Route> getRoutes(const Util::Network::Ip4::Ip4Address &sourceAddress);

    const Util::Network::Ip4::Ip4Route& findRoute(const Util::Network::Ip4::Ip4Address &sourceAddress, const Util::Network::Ip4::Ip4Address &address);

private:

    Util::Network::Ip4::Ip4Route defaultRoute;
    Util::ArrayList<Util::Network::Ip4::Ip4Route> routes;
    Util::Async::ReentrantSpinlock lock;
};

}
}
}

#endif
