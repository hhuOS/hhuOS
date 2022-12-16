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

#ifndef HHUOS_IP4ROUTINGMODULE_H
#define HHUOS_IP4ROUTINGMODULE_H

#include "lib/util/data/ArrayList.h"
#include "Ip4Route.h"
#include "lib/util/data/Array.h"
#include "lib/util/data/Collection.h"
#include "lib/util/data/Iterator.h"

namespace Network {
namespace Ip4 {
class Ip4Address;
}  // namespace Ip4
}  // namespace Network

namespace Network::Ip4 {

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

    void setDefaultRoute(const Ip4Route &route);

    void addRoute(const Ip4Route &route);

    void removeRoute(const Ip4Route &route);

    [[nodiscard]] const Ip4Route& findRouteTo(const Ip4Address &address) const;

private:

    Ip4Route defaultRoute;
    Util::Data::ArrayList<Ip4Route> routes;
};

}

#endif
