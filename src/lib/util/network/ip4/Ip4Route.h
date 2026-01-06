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
 *
 * The UDP/IP stack is based on a bachelor's thesis, written by Hannes Feil.
 * The original source code can be found here: https://github.com/hhuOS/hhuOS/tree/legacy/network
 */

#ifndef HHUOS_LIB_UTIL_NETWORK_IP4ROUTE_H
#define HHUOS_LIB_UTIL_NETWORK_IP4ROUTE_H

#include "util/base/String.h"
#include "util/network/ip4/Ip4Address.h"
#include "util/network/ip4/Ip4SubnetAddress.h"

namespace Util {
namespace Network {
namespace Ip4 {

/// Represents a route in the IPv4 routing table.
/// A route defines how packets should be forwarded to reach a specific destination.
/// It is made up of the following components:
///  - A source address, which is the IP address of the interface that is sending the packet.
///  - A target address, which is the destination subnet for the packet.
///  - An optional next hop address, which is the IP address of the next router to forward the packet to.
///    In this case, the packet cannot be sent directly to the target address, but rather needs to be sent to another
///    host that will forward it to the target address.
///  - A device identifier, which is the identifier of the network interface that should be used to send the packet.
///
///  For example, a route could look like this:
///  `10.0.2.0/24 device eth0 source 10.0.2.15`
///  This means that packets with a destination address in the subnet `10.0.2.0/24` should be sent
///  via the network interface `eth0` which has the source IP address `10.0.2.15`.
///
///  Another example could be:
///  `default via 10.0.2.2 device eth0 source 10.0.2.15`
///  This is a default route, which means that packets that do not match any other route should take this route.
///  In this case, packets will be sent to the next hop address `10.0.2.2` regardless of their destination address.
///  This is a typical setup for a gateway that forwards packets to the internet.
///  Again, the packets will be sent via the network interface `eth0` with the source IP address `10.0.2.15`.
class Ip4Route {

public:
    /// Create a new empty IPv4 route with default values.
    /// This corresponds to a route with a source address of `0.0.0.0` a target address of `0.0.0.0/32`,
    /// no next hop and an empty device identifier. Such a route is considered invalid.
    /// This constructor should not be used in most cases. However, it is used by the kernel to initialize
    /// the routing table and default route with empty values.
    ///
    /// ### Example
    /// ```c++
    /// const auto route = Ip4Route();
    /// const auto isValid = route.isValid(); // false
    /// ```
    Ip4Route() = default;

    /// Create a new IPv4 route with the given target address and device identifier.
    /// The source address is extracted from the target address and route has no next hop.
    ///
    /// ### Example
    /// ```c++
    /// const auto route = Ip4Route(Ip4SubnetAddress("10.0.2.15/24"), "eth0");
    ///
    /// const auto sourceAddress = route.getSourceAddress(); // 10.0.2.15
    /// const auto targetAddress = route.getTargetAddress(); // 10.0.2.0/24
    /// const auto deviceIdentifier = route.getDeviceIdentifier(); // "eth0"
    /// const auto hasNextHop = route.hasNextHop(); // false
    /// const auto nextHop = route.getNextHop(); // Panic: Route has no next hop!
    /// ```
    Ip4Route(const Ip4SubnetAddress &targetAddress, const String &deviceIdentifier) :
        sourceAddress(targetAddress.getIp4Address()), targetAddress(targetAddress),
        deviceIdentifier(deviceIdentifier) {}

    /// Create a new IPv4 route with the given source and target addresses and no next hop.
    ///
    /// ### Example
    /// ```c++
    /// const auto route = Ip4Route(Ip4Address("10.0.2.15"), Ip4SubnetAddress("10.0.2.0/24"), "eth0");
    ///
    /// const auto sourceAddress = route.getSourceAddress(); // 10.0.2.15
    /// const auto targetAddress = route.getTargetAddress(); // 10.0.2.0/24
    /// const auto deviceIdentifier = route.getDeviceIdentifier(); // "eth0"
    /// const auto hasNextHop = route.hasNextHop(); // false
    /// const auto nextHop = route.getNextHop(); // Panic: Route has no next hop!
    /// ```
    Ip4Route(const Ip4Address &sourceAddress, const Ip4SubnetAddress &targetAddress, const String &deviceIdentifier) :
        sourceAddress(sourceAddress), targetAddress(targetAddress), deviceIdentifier(deviceIdentifier) {}

    /// Create a new IPv4 route with the given target address, next hop address and device identifier.
    /// The source address is extracted from the target address.
    /// Every packet that matches the target address will be sent to the next hop address
    /// via the specified device identifier.
    ///
    /// ### Example
    /// ```c++
    /// const auto route = Ip4Route(Ip4SubnetAddress("10.0.2.15/24"), Ip4Address("10.0.2.2"), "eth0");
    ///
    /// const auto sourceAddress = route.getSourceAddress(); // 10.0.2.15
    /// const auto targetAddress = route.getTargetAddress(); // 10.0.2.0/24
    /// const auto deviceIdentifier = route.getDeviceIdentifier(); // "eth0"
    /// const auto hasNextHop = route.hasNextHop(); // true
    /// const auto nextHop = route.getNextHop(); // 10.0.2.2
    /// ```
    Ip4Route(const Ip4SubnetAddress &targetAddress, const Ip4Address &nextHop, const String &deviceIdentifier) :
        sourceAddress(targetAddress.getIp4Address()), targetAddress(targetAddress),
        nextHop(nextHop), deviceIdentifier(deviceIdentifier), nextHopValid(true) {}

    /// Create a new IPv4 route with the given source address, target address, next hop address and device identifier.
    /// Every packet that matches the target address will be sent to the next hop address
    /// via the specified device identifier.
    ///
    /// ### Example
    /// ```c++
    /// const auto route = Ip4Route(Ip4Address("10.0.2.15"), Ip4SubnetAddress("10.0.2.0/24"),
    ///     Ip4Address("10.0.2.2"), "eth0");
    ///
    /// const auto sourceAddress = route.getSourceAddress(); // 10.0.2.15
    /// const auto targetAddress = route.getTargetAddress(); // 10.0.2.0/24
    /// const auto deviceIdentifier = route.getDeviceIdentifier(); // "eth0"
    /// const auto hasNextHop = route.hasNextHop(); // true
    /// const auto nextHop = route.getNextHop(); // 10.0.2.2
    /// ```
    Ip4Route(const Ip4Address &sourceAddress, const Ip4SubnetAddress &targetAddress, const Ip4Address &nextHop,
        const String &deviceIdentifier) : sourceAddress(sourceAddress), targetAddress(targetAddress),
        nextHop(nextHop), deviceIdentifier(deviceIdentifier), nextHopValid(true) {}

    /// Compare this route with another route for equality.
    /// Two routes are considered equal if they have the same target address, device identifier and next hop address.
    ///
    /// ### Example
    /// ```c++
    /// const auto route1 = Ip4Route(Ip4SubnetAddress("10.0.2.15/24"), "eth0");
    /// const auto route2 = Ip4Route(Ip4Address("10.0.2.15"), Ip4SubnetAddress("10.0.2.0/24"), "eth0");
    /// const auto route3 = Ip4Route(Ip4SubnetAddress("10.0.2.15/24"), Ip4Address("10.0.2.2"), "eth0");
    ///
    /// const auto isEqual1 = (route1 == route2); // true, same target address and device identifier and no next hop
    /// const auto isEqual2 = (route1 == route3); // false, route3 has a next hop
    /// ```
    bool operator==(const Ip4Route &other) const {
        return targetAddress == other.targetAddress &&
            deviceIdentifier == other.deviceIdentifier &&
            nextHop == other.nextHop;
    }

    /// Compare this route with another route for inequality.
    /// Two routes are considered unequal if they have different target addresses,
    /// device identifiers or next hop addresses.
    ///
    /// ### Example
    /// ```c++
    /// const auto route1 = Ip4Route(Ip4SubnetAddress("10.0.2.15/24"), "eth0");
    /// const auto route2 = Ip4Route(Ip4Address("10.0.2.15"), Ip4SubnetAddress("10.0.2.0/24"), "eth0");
    /// const auto route3 = Ip4Route(Ip4SubnetAddress("10.0.2.15/24"), Ip4Address("10.0.2.2"), "eth0");
    ///
    /// const auto isNotEqual1 = (route1 != route2); // false, same target address and device identifier and no next hop
    /// const auto isNotEqual2 = (route1 != route3); // true, route3 has a next hop
    /// ```
    bool operator!=(const Ip4Route &other) const {
        return targetAddress != other.targetAddress ||
            deviceIdentifier != other.deviceIdentifier ||
            nextHop != other.nextHop;
    }

    /// Get the source address.
    const Ip4Address& getSourceAddress() const {
        return sourceAddress;
    }

    /// Get the target address.
    Ip4SubnetAddress getTargetAddress() const {
        return targetAddress;
    }

    /// Get the device identifier.
    const String& getDeviceIdentifier() const {
        return deviceIdentifier;
    }

    /// Check if this route has a next hop address.
    bool hasNextHop() const {
        return nextHopValid;
    }

    /// Get the next hop address.
    const Ip4Address& getNextHop() const {
        if (!nextHopValid) {
            Util::Panic::fire(Panic::UNSUPPORTED_OPERATION, "Ip4Route: Route has no next hop!");
        }

        return nextHop;
    }

    /// Check if this route is valid.
    /// A route is considered valid if it has a non-empty device identifier.
    bool isValid() const {
        return !deviceIdentifier.isEmpty();
    }

private:

    Ip4Address sourceAddress;
    Ip4SubnetAddress targetAddress;
    Ip4Address nextHop;
    String deviceIdentifier;

    bool nextHopValid = false;
};

}
}
}

#endif
