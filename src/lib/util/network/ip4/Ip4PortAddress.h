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

#ifndef HHUOS_LIB_UTIL_NETWORK_IP4PORTADDRESS_H
#define HHUOS_LIB_UTIL_NETWORK_IP4PORTADDRESS_H

#include <stdint.h>

#include "util/base/String.h"
#include "util/network/NetworkAddress.h"
#include "util/network/ip4/Ip4Address.h"

namespace Util {
namespace Network {
namespace Ip4 {

/// Represents an IPv4 address with a port number, used to identify a specific service on a device within an IP network.
/// The port number is a 16-bit unsigned integer value, that must be unique for each service running on the device.
/// Two services on the same device cannot use the same port number.
class Ip4PortAddress final : public NetworkAddress {

public:
    /// Create a new IPv4 port address with a zeroed buffer.
    /// This corresponds to the IP address `0.0.0.0:0`.
    ///
    /// ### Example
    /// ```c++
    /// const auto ipAddress = Ip4PortAddress();
    ///
    /// const auto length = ipAddress.getLength(); // 6
    /// const auto string = ipAddress.toString(); // "0.0.0.0:0"
    /// ```
    Ip4PortAddress() : NetworkAddress(ADDRESS_LENGTH, IP4_PORT) {}

    /// Create a new IPv4 port address from the given buffer.
    /// The buffer must be at least six bytes long.
    /// If the buffer is shorter, bytes will be read out of bounds, leading to undefined behavior.
    /// If the buffer is longer, only the first six bytes are copied.
    ///
    /// ### Example
    /// ```c++
    /// const uint8_t buffer1[6] = {1, 2, 3, 4, 1797};
    /// const uint8_t buffer2[8] = {5, 6, 7, 8, 1856, 9};
    /// const uint8_t buffer3[3] = {10, 11, 12};
    ///
    /// const auto ipAddress1 = Ip4PortAddress(buffer1); // 1.2.3.4:1797
    /// const auto ipAddress2 = Ip4PortAddress(buffer2); // 5.6.7.8:1856
    /// const auto ipAddress3 = Ip4PortAddress(buffer3); // Undefined behavior, buffer too short
    /// ```
    explicit Ip4PortAddress(const uint8_t *buffer) : NetworkAddress(buffer, ADDRESS_LENGTH, IP4_PORT) {}
    
    /// Create a new IPv4 port address from a string representation.
    /// The string must be in the format "X.X.X.X:Y", where each "X" is a decimal number between 0 and 255
    /// and "Y" is a decimal number between 0 and 65535.
    /// If the string is not in the correct format, a panic is fired.
    ///
    /// ### Example
    /// ```c++
    /// const auto ipAddress1 = Ip4Address("1.2.3.4:1797");
    /// const auto ipAddress2 = Ip4Address("5.6.7.8:1856");
    /// const auto ipAddress3 = Ip4Address("9.10.11"); // Panic: index out of bounds, string too short
    /// ```
    explicit Ip4PortAddress(const String &string) : NetworkAddress(ADDRESS_LENGTH, IP4_PORT) {
        const auto bufferAddress = Address(buffer);

        if (string.beginsWith(":")) {
            // Buffer is already initialized with "0.0.0.0:0" -> Just set the port number
            const auto port = String::parseNumber<uint16_t>(string.substring(1));
            bufferAddress.write16(port, Ip4Address::ADDRESS_LENGTH);
        } else {
            const auto split = string.split(":");
            const auto ip4Address = Ip4Address(split[0]);
            const auto port = split.length() > 1 ? String::parseNumber<uint16_t>(split[1]) : 0;

            // Write the IP address and port number to the buffer
            ip4Address.getAddress(buffer);
            bufferAddress.write16(port, Ip4Address::ADDRESS_LENGTH);
        }
    }

    /// Create a new IPv4 port address from an IPv4 address and a port number
    ///
    /// ### Example
    /// ```c++
    /// const auto ipAddress = Ip4Address("1.2.3.4");
    ///
    /// const auto portAddress1 = Ip4PortAddress(ipAddress, 1797); // 1.2.3.4:1797
    /// const auto portAddress2 = Ip4PortAddress(ipAddress, 1856); // 1.2.3.4:1856
    /// ```
    Ip4PortAddress(const Ip4Address &address, const uint16_t port) :
        NetworkAddress(Ip4Address::ADDRESS_LENGTH + 2, IP4_PORT)
    {
        // Copy the IP address to the buffer and write the port number
        address.getAddress(buffer);
        Address(buffer).write16(port, Ip4Address::ADDRESS_LENGTH);
    }

    /// Create a new IPv4 port address from an IPv4 address with a default port number of 0.
    ///
    /// ### Example
    /// ```c++
    /// const auto ipAddress = Ip4Address("1.2.3.4");
    /// const auto portAddress = Ip4PortAddress(ipAddress); // 1.2.3.4:0
    /// ```
    explicit Ip4PortAddress(const Ip4Address &address) : NetworkAddress(ADDRESS_LENGTH, IP4_PORT) {
        // Copy the IP address to the buffer (port is already initialized to 0)
        address.getAddress(buffer);
    }

    /// Create a new IPv4 port address with the IPv4 address `0.0.0.0` and a port number.
    ///
    /// ### Example
    /// ```c++
    /// const auto portAddress1 = Ip4PortAddress(1797); // 0.0.0.0:1797
    /// const auto portAddress2 = Ip4PortAddress(1856); // 0.0.0.0:1856
    /// ```
    explicit Ip4PortAddress(const uint16_t port) : NetworkAddress(ADDRESS_LENGTH, IP4_PORT) {
        // Buffer is already initialized with "0.0.0.0:0" -> Just set the port number
        Address(buffer).write16(port, Ip4Address::ADDRESS_LENGTH);
    }

    /// Get the IPv4 address without the port number.
    ///
    /// ### Example
    /// ```c++
    /// const auto portAddress = Ip4PortAddress("1.2.3.4:1797");
    /// const auto ipAddress = portAddress.getIp4Address(); // 1.2.3.4
    /// ```
    Ip4Address getIp4Address() const {
        return Ip4Address(buffer);
    }

    /// Get the port number of this address.
    ///
    /// ### Example
    /// ```c++
    /// const auto portAddress = Ip4PortAddress("1.2.3.4:1797");
    /// const auto port = portAddress.getPort(); // 1797
    /// ```
    uint16_t getPort() const {
        return Address(buffer).read16(Ip4Address::ADDRESS_LENGTH);
    }

    /// Set the port number of this address, overwriting the previous value.
    ///
    /// ### Example
    /// ```c++
    /// const auto portAddress = Ip4PortAddress("1.2.3.4:1797");
    /// auto port = portAddress.getPort(); // 1797
    ///
    /// portAddress.setPort(1856);
    /// port = portAddress.getPort(); // 1856
    /// ```
    void setPort(uint16_t port) const {
        Address(buffer).write16(port, Ip4Address::ADDRESS_LENGTH);
    }

    /// Create a new on-heap instance as a copy of this address.
    ///
    /// ### Example
    /// ```c++
    /// const auto ipAddress1 = Ip4Address("1.2.3.4:1797");
    /// const auto *ipAddress2 = ipAddress1.createCopy();
    ///
    /// const auto isEqual = (ipAddress1 == *ipAddress2); // true
    /// ```
    NetworkAddress* createCopy() const override {
        return new Ip4PortAddress(*this);
    }

    /// Create a string representation of the IPv4 port address in the format "X.X.X.X:Y".
    ///
    /// ### Example
    /// ```c++
    /// const auto ipAddress1 = Ip4Address("1.2.3.4:1797");
    /// const auto string1 = mac1.toString(); // "1.2.3.4:1797"
    /// const auto ipAddress2 = Ip4Address(string1);
    ///
    /// const auto isEqual = (ipAddress1 == ipAddress2); // true
    /// ```
    String toString() const override {
        return String::format("%u.%u.%u.%u:%u", buffer[0], buffer[1], buffer[2], buffer[3], getPort());
    }

    /// The length in bytes of an IPv4 port address.
    static constexpr uint32_t ADDRESS_LENGTH = Ip4Address::ADDRESS_LENGTH + sizeof(uint16_t);
};

}
}
}

#endif
