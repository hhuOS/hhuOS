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
 */

#include "Route.h"

#include "lib/util/base/System.h"
#include "lib/util/network/ip4/Ip4Address.h"
#include "lib/util/network/ip4/Ip4Route.h"
#include "lib/util/network/Socket.h"
#include "lib/util/io/stream/PrintStream.h"
#include "lib/util/network/ip4/Ip4SubnetAddress.h"
#include "lib/util/async/Process.h"

Route::Route(const Util::Array<Util::String> &arguments) : arguments(arguments) {}

int32_t Route::parse() {
    if (arguments.length() == 0 || Util::String(COMMAND_SHOW).beginsWith(arguments[0])) {
        printRoutes(arguments.length() > 1 ? Util::Network::Ip4::Ip4Address(arguments[1]) : Util::Network::Ip4::Ip4Address::ANY);
        return 0;
    } else if (Util::String(COMMAND_REMOVE).beginsWith(arguments[0])) {
        return remove(parseRoute(arguments));
    } else if (Util::String(COMMAND_ADD).beginsWith(arguments[0])) {
        return add(parseRoute(arguments));
    }

    Util::System::error << "ip: Invalid argument '" << arguments[0] << "'!" << Util::Io::PrintStream::ln << Util::Io::PrintStream::flush;
    return -1;
}

int32_t Route::remove(const Util::Network::Ip4::Ip4Route &route) {
    const auto &address = Util::Network::Ip4::Ip4Address::ANY;
    auto ipSocket = Util::Network::Socket::createSocket(Util::Network::Socket::IP4);
    if (!ipSocket.bind(address)) {
        Util::System::error << "ip: Unable to bind IPv4 socket to address '" << address.toString() << "'!" << Util::Io::PrintStream::ln << Util::Io::PrintStream::flush;
        return -1;
    }

    if (!ipSocket.removeRoute(route)) {
        Util::System::error << "ip: Unable to remove route!" << Util::Io::PrintStream::ln << Util::Io::PrintStream::flush;
        return -1;
    }

    return 0;
}

int32_t Route::add(const Util::Network::Ip4::Ip4Route &route) {
    const auto &address = Util::Network::Ip4::Ip4Address::ANY;
    auto ipSocket = Util::Network::Socket::createSocket(Util::Network::Socket::IP4);
    if (!ipSocket.bind(address)) {
        Util::System::error << "ip: Unable to bind IPv4 socket to address '" << address.toString() << "'!" << Util::Io::PrintStream::ln << Util::Io::PrintStream::flush;
        return -1;
    }

    if (!ipSocket.addRoute(route)) {
        Util::System::error << "ip: Unable to add route!" << Util::Io::PrintStream::ln << Util::Io::PrintStream::flush;
        return -1;
    }

    return 0;
}

Util::Network::Ip4::Ip4Route Route::parseRoute(const Util::Array<Util::String> &arguments) {
    if (arguments.length() < 2) {
        Util::System::error << "ip: Missing arguments for 'route " << COMMAND_ADD << "'!" << Util::Io::PrintStream::ln << Util::Io::PrintStream::flush;
        Util::Async::Process::exit(-1);
    }

    auto route = Util::Network::Ip4::Ip4Route();
    if (arguments.length() == 3) { // route [add | delete] [target address] [device]
        const auto targetAddress = Util::Network::Ip4::Ip4SubnetAddress(arguments[1]);
        const auto &deviceIdentifier = arguments[2];
        route = Util::Network::Ip4::Ip4Route(targetAddress, deviceIdentifier);
    } else if (arguments.length() == 4) {
        if (arguments[1].contains('/')) { // route [add | delete] [target address] [next hop] [device]
            const auto targetAddress = Util::Network::Ip4::Ip4SubnetAddress(arguments[1]);
            const auto nextHop = Util::Network::Ip4::Ip4Address(arguments[2]);
            const auto &deviceIdentifier = arguments[3];
            route = Util::Network::Ip4::Ip4Route(targetAddress, nextHop, deviceIdentifier);
        } else { // route [add | delete] [source address] [target address] [device]
            const auto sourceAddress = Util::Network::Ip4::Ip4Address(arguments[1]);
            const auto targetAddress = Util::Network::Ip4::Ip4SubnetAddress(arguments[2]);
            const auto &deviceIdentifier = arguments[3];
            route = Util::Network::Ip4::Ip4Route(sourceAddress, targetAddress, deviceIdentifier);
        }
    } else if (arguments.length() == 5) { // route [add | delete] [source address] [target address] [next hop] [device]
        const auto sourceAddress = Util::Network::Ip4::Ip4Address(arguments[1]);
        const auto targetAddress = Util::Network::Ip4::Ip4SubnetAddress(arguments[2]);
        const auto nextHop = Util::Network::Ip4::Ip4Address(arguments[3]);
        const auto &deviceIdentifier = arguments[4];
        route = Util::Network::Ip4::Ip4Route(sourceAddress, targetAddress, nextHop, deviceIdentifier);
    }

    return route;
}

int32_t Route::printRoutes(const Util::Network::Ip4::Ip4Address &address) {
    auto ipSocket = Util::Network::Socket::createSocket(Util::Network::Socket::IP4);
    if (!ipSocket.bind(address)) {
        Util::System::error << "ip: Unable to bind IPv4 socket to address '" << address.toString() << "'!" << Util::Io::PrintStream::ln << Util::Io::PrintStream::flush;
        return -1;
    }

    auto routes = ipSocket.getRoutes();

    for (const auto &route : routes) {
        Util::System::out << (route.getTargetAddress().getBitCount() == 0 ? "default" : route.getTargetAddress().toString());
        if (route.hasNextHop()) Util::System::out << " via " << route.getNextHop().toString();
        Util::System::out << " device " << route.getDeviceIdentifier()
                          << " source " << route.getSourceAddress().toString() << Util::Io::PrintStream::ln;
    }

    Util::System::out << Util::Io::PrintStream::flush;
    return 0;
}
