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

#include "Route.h"

#include "lib/util/base/System.h"
#include "lib/util/network/ip4/Ip4Address.h"
#include "lib/util/network/ip4/Ip4Route.h"
#include "lib/util/network/Socket.h"
#include "lib/util/io/stream/PrintStream.h"
#include "lib/util/network/ip4/Ip4SubnetAddress.h"

Route::Route(const Util::Array<Util::String> &arguments) : arguments(arguments) {}

int32_t Route::parse() {
    if (arguments.length() == 0 || Util::String(COMMAND_SHOW).beginsWith(arguments[0])) {
        return show();
    } else if (Util::String(COMMAND_REMOVE).beginsWith(arguments[0])) {
        return remove();
    } else if (Util::String(COMMAND_ADD).beginsWith(arguments[0])) {
        return add();
    }

    Util::System::error << "ip: Invalid argument '" << arguments[0] << "'!" << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
    return -1;
}

int32_t Route::show() {
    if (arguments.length() > 1) {
        printRoutes(Util::Network::Ip4::Ip4Address(arguments[1]));
    } else {
        printRoutes(Util::Network::Ip4::Ip4Address::ANY);
    }

    return 0;
}

int32_t Route::remove() {
    if (arguments.length() < 3) {
        Util::System::error << "ip: Missing arguments for 'route " << COMMAND_REMOVE << "'!" << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
        return - 1;
    }

    auto route = Util::Network::Ip4::Ip4Route();
    if (arguments.length() == 3) {
        auto address = Util::Network::Ip4::Ip4SubnetAddress(arguments[1]);
        auto deviceIdentifier = arguments[2];
        route = Util::Network::Ip4::Ip4Route(address, deviceIdentifier);
    } else {
        auto address = Util::Network::Ip4::Ip4SubnetAddress(arguments[1]);
        auto nextHop = Util::Network::Ip4::Ip4Address(arguments[2]);
        auto deviceIdentifier = arguments[3];
        route = Util::Network::Ip4::Ip4Route(address, nextHop, deviceIdentifier);
    }

    const auto &address = Util::Network::Ip4::Ip4Address::ANY;
    auto ipSocket = Util::Network::Socket::createSocket(Util::Network::Socket::IP4);
    if (!ipSocket.bind(address)) {
        Util::System::error << "ip: Unable to bind IPv4 socket to address '" << address.toString() << "'!" << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
        return -1;
    }

    if (!ipSocket.removeRoute(route)) {
        Util::System::error << "ip: Unable to remove route!" << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
        return -1;
    }

    return 0;
}

int32_t Route::add() {
    if (arguments.length() < 3) {
        Util::System::error << "ip: Missing arguments for 'route " << COMMAND_ADD << "'!" << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
        return - 1;
    }

    auto route = Util::Network::Ip4::Ip4Route();
    if (arguments.length() == 3) {
        auto address = Util::Network::Ip4::Ip4SubnetAddress(arguments[1]);
        auto deviceIdentifier = arguments[2];
        route = Util::Network::Ip4::Ip4Route(address, deviceIdentifier);
    } else {
        auto address = Util::Network::Ip4::Ip4SubnetAddress(arguments[1]);
        auto nextHop = Util::Network::Ip4::Ip4Address(arguments[2]);
        auto deviceIdentifier = arguments[3];
        route = Util::Network::Ip4::Ip4Route(address, nextHop, deviceIdentifier);
    }

    const auto &address = Util::Network::Ip4::Ip4Address::ANY;
    auto ipSocket = Util::Network::Socket::createSocket(Util::Network::Socket::IP4);
    if (!ipSocket.bind(address)) {
        Util::System::error << "ip: Unable to bind IPv4 socket to address '" << address.toString() << "'!" << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
        return -1;
    }

    if (!ipSocket.addRoute(route)) {
        Util::System::error << "ip: Unable to add route!" << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
        return -1;
    }

    return 0;
}

int32_t Route::printRoutes(const Util::Network::Ip4::Ip4Address &address) {
    auto ipSocket = Util::Network::Socket::createSocket(Util::Network::Socket::IP4);
    if (!ipSocket.bind(address)) {
        Util::System::error << "ip: Unable to bind IPv4 socket to address '" << address.toString() << "'!" << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
        return -1;
    }

    auto routes = ipSocket.getRoutes();

    for (const auto &route : routes) {
        Util::System::out << (route.getAddress().getBitCount() == 0 ? "default" : route.getTargetAddress().toString());
        if (route.hasNextHop()) Util::System::out << " via " << route.getNextHop().toString();
        Util::System::out << " device " << route.getDeviceIdentifier()
                          << " source " << route.getSourceAddress().toString() << Util::Io::PrintStream::endl;
    }

    Util::System::out << Util::Io::PrintStream::flush;
    return 0;
}
