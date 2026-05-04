/*
 * Copyright (C) 2017-2026 Heinrich Heine University Düsseldorf,
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
 */

#ifndef HHUOS_APPLICATION_IP_ROUTE_H
#define HHUOS_APPLICATION_IP_ROUTE_H

#include <stdint.h>

#include <util/collection/Array.h>
#include <util/base/String.h>
#include <util/network/ip4/Ip4Route.h>

/// Subcommand for the `ip` program to show and modify IPv4 routes of network devices.
class Route {

public:
    /// Create a new instance from the program arguments given to `ip` for this subcommand.
    /// For example, if the user calls `ip route show eth0`, the `ip` program passes `[show, eth0]` to this subcommand.
    explicit Route(const Util::Array<Util::String> &arguments);

    /// Parse the arguments given in the constructor and execute the subcommand.
    /// On success, 0 is returned.
    int32_t parse() const;

    /// The name of this subcommand (used for argument parsing)
    static constexpr const char *COMMAND = "route";

private:

    static int32_t printRoutes(const Util::Network::Ip4::Ip4Address &address);

    static int32_t remove(const Util::Network::Ip4::Ip4Route &route);

    static int32_t add(const Util::Network::Ip4::Ip4Route &route);

    static Util::Network::Ip4::Ip4Route parseRoute(const Util::Array<Util::String> &arguments);

    const Util::Array<Util::String> arguments;

    static constexpr const char *COMMAND_SHOW = "show";
    static constexpr const char *COMMAND_REMOVE = "delete";
    static constexpr const char *COMMAND_ADD = "add";
};


#endif
