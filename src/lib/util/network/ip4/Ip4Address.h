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

#ifndef HHUOS_LIB_UTIL_NETWORK_IP4ADDRESS_H
#define HHUOS_LIB_UTIL_NETWORK_IP4ADDRESS_H

#include <stdint.h>

#include "util/base/String.h"
#include "util/network/NetworkAddress.h"

namespace Util {
namespace Network {
namespace Ip4 {

/// Represents an IPv4 address, which is 32-bit long and identifies a device on an IP network.
/// An IP address is not tied to a specific hardware interface,
/// but rather assigned to a network interface by the operating system.
/// This is a subclass of `NetworkAddress` that provides specific functionality for IPv4 addresses,
/// such as creating an instance from a string.
class Ip4Address final : public NetworkAddress {

public:
    /// Create a new IPv4 address with a zeroed buffer.
    /// This corresponds to the IP address `0.0.0.0`.
    ///
    /// ### Example
    /// ```c++
    /// const auto ipAddress = Ip4Address();
    ///
    /// const auto length = ipAddress.getLength(); // 4
    /// const auto string = ipAddress.toString(); // "0.0.0.0"
    /// const auto isBroadcast = ipAddress.isBroadcastAddress(); // false
    /// ```
    Ip4Address() : NetworkAddress(ADDRESS_LENGTH, IP4) {}

    /// Create a new IPv4 address from the given buffer.
    /// The buffer must be at least four bytes long.
    /// If the buffer is shorter, bytes will be read out of bounds, leading to undefined behavior.
    /// If the buffer is longer, only the first four bytes are copied.
    ///
    /// ### Example
    /// ```c++
    /// const uint8_t buffer1[4] = {1, 2, 3, 4};
    /// const uint8_t buffer2[6] = {5, 6, 7, 8, 9, 10};
    /// const uint8_t buffer3[3] = {11, 12, 13};
    ///
    /// const auto ipAddress1 = Ip4Address(buffer1); // 1.2.3.4
    /// const auto ipAddress2 = Ip4Address(buffer2); // 5.6.7.8
    /// const auto ipAddress3 = Ip4Address(buffer3); // Undefined behavior, buffer too short
    /// ```
    explicit Ip4Address(const uint8_t *buffer) : NetworkAddress(buffer, ADDRESS_LENGTH, IP4) {}

    /// Create a new IPv4 address from a string representation.
    /// The string must be in the format "X.X.X.X", where each "X" is a decimal number between 0 and 255.
    /// If the string is not in the correct format, a panic is fired.
    ///
    /// ### Example
    /// ```c++
    /// const auto ipAddress1 = Ip4Address("1.2.3.4");
    /// const auto ipAddress2 = Ip4Address("5.6.7.8");
    /// const auto ipAddress3 = Ip4Address("9.10.11"); // Panic: index out of bounds, string too short
    /// ```
    explicit Ip4Address(const String &string) : NetworkAddress(ADDRESS_LENGTH, IP4) {
        auto split = string.split(".");
        const uint8_t buffer[4] = {
            String::parseNumber<uint8_t>(split[0]),
            String::parseNumber<uint8_t>(split[1]),
            String::parseNumber<uint8_t>(split[2]),
            String::parseNumber<uint8_t>(split[3]),
        };

        setAddress(buffer);
    }

    /// Check if this IPv4 address is a broadcast address (`255.255.255.255`).
    ///
    /// ### Example
    /// ```c++
    /// const auto ipAddress1 = Ip4Address("1.2.3.4");
    /// const auto ipAddress2 = Ip4Address::BROADCAST;
    /// const auto ipAddress3 = Ip4Address("255.255.255.255");
    ///
    /// const auto isBroadcast1 = ipAddress1.isBroadcastAddress(); // false
    /// const auto isBroadcast2 = ipAddress2.isBroadcastAddress(); // true
    /// const auto isBroadcast3 = ipAddress3.isBroadcastAddress(); // true
    /// ```
    bool isBroadcastAddress() const {
        return Address(buffer).compareRange(Address(BROADCAST.buffer), ADDRESS_LENGTH) == 0;
    }

    /// Create a new on-heap instance as a copy of this address.
    ///
    /// ### Example
    /// ```c++
    /// const auto ipAddress1 = Ip4Address("1.2.3.4");
    /// const auto *ipAddress2 = ipAddress1.createCopy();
    ///
    /// const auto isEqual = (ipAddress1 == *ipAddress2); // true
    /// ```
    NetworkAddress* createCopy() const override {
        return new Ip4Address(*this);
    }

    /// Create a string representation of the IPv4 address in the format "X.X.X.X".
    ///
    /// ### Example
    /// ```c++
    /// const auto ipAddress1 = Ip4Address("1.2.3.4");
    /// const auto string1 = mac1.toString(); // "1.2.3.4"
    /// const auto ipAddress2 = Ip4Address(string1);
    ///
    /// const auto isEqual = (ipAddress1 == ipAddress2); // true
    /// ```
    String toString() const override {
        return String::format("%u.%u.%u.%u", buffer[0], buffer[1], buffer[2], buffer[3]);
    }

    /// A constant storing the IPv4 address `0.0.0.0`, which can for example be used to listen on all interfaces.
    ///
    /// /// ### Example
    /// ```c++
    /// const auto length = Ip4Address::ANY.getLength(); // 4
    /// const auto string = Ip4Address::ANY.toString(); // "0.0.0.0"
    /// const auto isBroadcast = Ip4Address::ANY.isBroadcastAddress(); // false
    /// ```
    static const Ip4Address ANY;

    /// A constant storing the broadcast IPv4 address, which is always `255.255.255.255`.
    /// The broadcast address is used to send packets to all devices on the local network segment.
    /// It is a special address that cannot be assigned to a network interface.
    ///
    /// ### Example
    /// ```c++
    /// const auto length = Ip4Address::BROADCAST.getLength(); // 4
    /// const auto string = Ip4Address::BROADCAST.toString(); // "255.255.255.255"
    /// const auto isBroadcast = Ip4Address::BROADCAST.isBroadcastAddress(); // true
    /// ```
    static const Ip4Address BROADCAST;

    /// The length in bytes of an IPv4 address.
    static constexpr uint8_t ADDRESS_LENGTH = 4;
};

}
}
}

#endif
