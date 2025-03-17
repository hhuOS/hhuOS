/*
 * Copyright (C) 2018-2025 Heinrich-Heine-Universitaet Duesseldorf,
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

#ifndef HHUOS_ROUTE_H
#define HHUOS_ROUTE_H

#include <stdint.h>

#include "lib/util/collection/Array.h"
#include "lib/util/base/String.h"
#include "lib/util/network/ip4/Ip4Route.h"

namespace Util {
namespace Network {
namespace Ip4 {
class Ip4Address;
}  // namespace Ip4
}  // namespace Network
}  // namespace Util

class Route {

public:
    /**
     * Constructor.
     */
    explicit Route(const Util::Array<Util::String> &arguments);

    /**
     * Copy Constructor.
     */
    Route(const Route &other) = delete;

    /**
     * Assignment operator.
     */
    Route &operator=(const Route &other) = delete;

    /**
     * Destructor.
     */
    ~Route() = default;

    static const constexpr char *COMMAND = "route";

    int32_t parse();

private:

    static int32_t printRoutes(const Util::Network::Ip4::Ip4Address &address);

    static int32_t remove(const Util::Network::Ip4::Ip4Route &route);

    static int32_t add(const Util::Network::Ip4::Ip4Route &route);

    static Util::Network::Ip4::Ip4Route parseRoute(const Util::Array<Util::String> &arguments);

    const Util::Array<Util::String> arguments;

    static const constexpr char *COMMAND_SHOW = "show";
    static const constexpr char *COMMAND_REMOVE = "delete";
    static const constexpr char *COMMAND_ADD = "add";
};


#endif
