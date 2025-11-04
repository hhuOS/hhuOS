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

#ifndef HHUOS_LIB_UTIL_NETWORK_IP4PORTADDRESS_H
#define HHUOS_LIB_UTIL_NETWORK_IP4PORTADDRESS_H

#include <stdint.h>

#include "util/base/String.h"
#include "util/network/NetworkAddress.h"
#include "util/network/ip4/Ip4Address.h"

namespace Util::Network::Ip4 {

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
    Ip4PortAddress();

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
    explicit Ip4PortAddress(const uint8_t *buffer);
    
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
    explicit Ip4PortAddress(const String &string);

    /// Create a new IPv4 port address from an IPv4 address and a port number
    ///
    /// ### Example
    /// ```c++
    /// const auto ipAddress = Ip4Address("1.2.3.4");
    ///
    /// const auto portAddress1 = Ip4PortAddress(ipAddress, 1797); // 1.2.3.4:1797
    /// const auto portAddress2 = Ip4PortAddress(ipAddress, 1856); // 1.2.3.4:1856
    /// ```
    Ip4PortAddress(const Ip4Address &address, uint16_t port);

    /// Create a new IPv4 port address from an IPv4 address with a default port number of 0.
    ///
    /// ### Example
    /// ```c++
    /// const auto ipAddress = Ip4Address("1.2.3.4");
    /// const auto portAddress = Ip4PortAddress(ipAddress); // 1.2.3.4:0
    /// ```
    explicit Ip4PortAddress(const Ip4Address &address);

    /// Create a new IPv4 port address with the IPv4 address `0.0.0.0` and a port number.
    ///
    /// ### Example
    /// ```c++
    /// const auto portAddress1 = Ip4PortAddress(1797); // 0.0.0.0:1797
    /// const auto portAddress2 = Ip4PortAddress(1856); // 0.0.0.0:1856
    /// ```
    explicit Ip4PortAddress(uint16_t port);

    /// Get the IPv4 address without the port number.
    ///
    /// ### Example
    /// ```c++
    /// const auto portAddress = Ip4PortAddress("1.2.3.4:1797");
    /// const auto ipAddress = portAddress.getIp4Address(); // 1.2.3.4
    /// ```
    [[nodiscard]] Ip4Address getIp4Address() const;

    /// Get the port number of this address.
    ///
    /// ### Example
    /// ```c++
    /// const auto portAddress = Ip4PortAddress("1.2.3.4:1797");
    /// const auto port = portAddress.getPort(); // 1797
    /// ```
    [[nodiscard]] uint16_t getPort() const;

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
    void setPort(uint16_t port) const;

    /// Create a new on-heap instance as a copy of this address.
    ///
    /// ### Example
    /// ```c++
    /// const auto ipAddress1 = Ip4Address("1.2.3.4:1797");
    /// const auto *ipAddress2 = ipAddress1.createCopy();
    ///
    /// const auto isEqual = (ipAddress1 == *ipAddress2); // true
    /// ```
    [[nodiscard]] NetworkAddress* createCopy() const override;

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
    [[nodiscard]] String toString() const override;

    /// The length in bytes of an IPv4 port address.
    static constexpr uint32_t ADDRESS_LENGTH = Ip4Address::ADDRESS_LENGTH + sizeof(uint16_t);
};

}

#endif
