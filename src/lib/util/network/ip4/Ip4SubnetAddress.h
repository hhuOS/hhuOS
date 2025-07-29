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

#ifndef HHUOS_LIB_UTIL_NETWORK_IP4SUBNETADDRESS_H
#define HHUOS_LIB_UTIL_NETWORK_IP4SUBNETADDRESS_H

#include <stdint.h>

#include "base/String.h"
#include "network/NetworkAddress.h"
#include "network/ip4/Ip4Address.h"

namespace Util::Network::Ip4 {

/// Represents an IPv4 address with a subnet mask, which is used to identify a range of IP addresses within a network.
/// It is implemented as a combination of a 32-bit IPv4 address and a bit count (0-32),
/// that specifies the number of bits used for the network portion of the address.
///
/// For example, the address `10.0.2.15/24` represents the network interface with the IP address `10.0.2.15`
/// in the `10.0.2.0` subnet. The bit count `24` indicates that the first 24 bits of the address are used to
/// identify the subnet, while the remaining 8 bits are used for host addresses within that subnet. In this case,
/// the subnet can accommodate up to 255 addresses (from `10.0.2.0` to `10.0.2.254`),
/// with `10.0.2.255` being the broadcast address.
class Ip4SubnetAddress final : public NetworkAddress {

public:
    /// Create a new IPv4 subnet address with a zeroed buffer.
    /// This corresponds to the IP address `0.0.0.0/0`.
    ///
    /// ### Example
    /// ```c++
    /// const auto ipAddress = Ip4Address();
    ///
    /// const auto length = ipAddress.getLength(); // 5
    /// const auto string = ipAddress.toString(); // "0.0.0.0/0"
    /// ```
    Ip4SubnetAddress();

    /// Create a new IPv4 subnet address from the given buffer.
    /// The buffer must be at least five bytes long.
    /// If the buffer is shorter, bytes will be read out of bounds, leading to undefined behavior.
    /// If the buffer is longer, only the first five bytes are copied.
    ///
    /// ### Example
    /// ```c++
    /// const uint8_t buffer1[5] = {1, 2, 3, 4, 24};
    /// const uint8_t buffer2[7] = {5, 6, 7, 8, 16, 9};
    /// const uint8_t buffer3[4] = {10, 11, 12, 13};
    /// const uint8_t buffer3[5] = {14, 15, 16, 17, 48};
    ///
    /// const auto ipAddress1 = Ip4Address(buffer1); // 1.2.3.4/24
    /// const auto ipAddress2 = Ip4Address(buffer2); // 5.6.7.8/16
    /// const auto ipAddress3 = Ip4Address(buffer3); // Undefined behavior, buffer too short
    /// const auto ipAddress4 = Ip4Address(buffer4); // Panic: bit count must be between 0 and 32
    /// ```
    explicit Ip4SubnetAddress(const uint8_t *buffer);

    /// Create a new IPv4 subnet address from a string representation.
    /// The string must be in the format "X.X.X.X/Y", where each "X" is a decimal number between 0 and 255
    /// and "Y" is the bit count (0-32) that specifies the number of bits used for the network portion of the address.
    /// If the string is not in the correct format, a panic is fired.
    ///
    /// ### Example
    /// ```c++
    /// const auto ipAddress1 = Ip4Address("1.2.3.4/24");
    /// const auto ipAddress2 = Ip4Address("5.6.7.8/16");
    /// const auto ipAddress3 = Ip4Address("9.10.11"); // Panic: index out of bounds, string too short
    /// const auto ipAddress4 = Ip4Address("12.13.14.15/48"); // Panic: bit count must be between 0 and 32
    /// ```
    explicit Ip4SubnetAddress(const String &string);

    /// Create a new IPv4 subnet address from an IPv4 address and a bit count (0-32).
    ///
    /// ### Example
    /// ```c++
    /// const auto ipAddress = Ip4Address("1.2.3.4");
    ///
    /// const auto subnetAddress1 = Ip4SubnetAddress(ipAddress, 24); // 1.2.3.4/24
    /// const auto subnetAddress2 = Ip4SubnetAddress(ipAddress, 16); // 1.2.3.4/16
    /// const auto subnetAddress3 = Ip4SubnetAddress(ipAddress, 48); // Panic: bit count must be between 0 and 32
    /// ```
    Ip4SubnetAddress(const Ip4Address &address, uint8_t bitCount);

    /// Create a new IPv4 subnet address from an IPv4 address with a default bit count of 32.
    /// Such a subnet address identifies a single host in the network, as it uses all 32 bits for the address.
    ///
    /// ### Example
    /// ```c++
    /// const auto ipAddress = Ip4Address("1.2.3.4");
    /// const auto subnetAddress = Ip4SubnetAddress(ipAddress); // 1.2.3.4/32
    /// ```
    explicit Ip4SubnetAddress(const Ip4Address &address);

    /// Create a new IPv4 subnet address with the IPv4 address `0.0.0.0` and a given bit count (0-32).
    ///
    /// ### Example
    /// ```c++
    /// const auto subnetAddress1 = Ip4SubnetAddress(24); // 0.0.0.0/24
    /// const auto subnetAddress2 = Ip4SubnetAddress(16); // 0.0.0.0/16
    /// const auto subnetAddress3 = Ip4SubnetAddress(48); // Panic: bit count must be between 0 and 32
    /// ```
    explicit Ip4SubnetAddress(uint8_t bitCount);

    /// Get the IPv4 address of this subnet address. That is the IP address without the subnet mask.
    ///
    /// ### Example
    /// ```c++
    /// const auto subnetAddress = Ip4SubnetAddress("1.2.3.4/24");
    /// const auto ipAddress = subnetAddress.getIp4Address(); // 1.2.3.4
    /// ```
    [[nodiscard]] Ip4Address getIp4Address() const;

    /// Get the bit count of this subnet address.
    ///
    /// ### Example
    /// ```c++
    /// const auto subnetAddress = Ip4SubnetAddress("1.2.3.4/24");
    /// const auto bitCount = subnetAddress.getBitCount(); // 24
    /// ```
    [[nodiscard]] uint8_t getBitCount() const;

    /// Create a new IPv4 subnet address that represents the subnet portion of this address.
    /// For example, if the address is `10.0.2.15/24`, the subnet address will be `10.0.2.0/24`.
    ///
    /// ### Example
    /// ```c++
    /// const auto ipAddress1 = Ip4SubnetAddress("1.2.3.4/24");
    /// const auto ipAddress2 = Ip4SubnetAddress("5.6.7.8/16");
    ///
    /// const auto subnetAddress1 = ipAddress1.getSubnetAddress(); // 1.2.3.0/24
    /// const auto subnetAddress2 = ipAddress2.getSubnetAddress(); // 5.6.0.0/16
    /// ```
    [[nodiscard]] Ip4SubnetAddress getSubnetAddress() const;

    /// Create a new IPv4 address that represents the broadcast address of this subnet.
    /// That is the highest address in the subnet, and is used to send packets to all hosts in the subnet.
    /// For example, if the address is `10.0.2.15/24` the broadcast address will be `10.0.2.255`.
    ///
    /// ### Example
    /// ```c++
    /// const auto ipAddress1 = Ip4SubnetAddress("1.2.3.4/24");
    /// const auto ipAddress2 = Ip4SubnetAddress("5.6.7.8/16");
    /// const auto ipAddress3 = Ip4SubnetAddress("9.10.11.12/32");
    ///
    /// const auto broadcastAddress1 = ipAddress1.getBroadcastAddress(); // 1.2.3.255
    /// const auto broadcastAddress2 = ipAddress2.getBroadcastAddress(); // 5.6.255.255
    /// const auto broadcastAddress3 = ipAddress3.getBroadcastAddress(); // 9.10.11.12 -> Only one host in the subnet
    /// ```
    [[nodiscard]] Ip4Address getBroadcastAddress() const;

    /// Create a new on-heap instance as a copy of this address.
    ///
    /// ### Example
    /// ```c++
    /// const auto ipAddress1 = Ip4Address("1.2.3.4/24");
    /// const auto *ipAddress2 = ipAddress1.createCopy();
    ///
    /// const auto isEqual = (ipAddress1 == *ipAddress2); // true
    /// ```
    [[nodiscard]] NetworkAddress *createCopy() const override;

    /// Create a string representation of the IPv4 subnet address in the format "X.X.X.X/Y".
    ///
    /// ### Example
    /// ```c++
    /// const auto ipAddress1 = Ip4Address("1.2.3.4/24");
    /// const auto string1 = mac1.toString(); // "1.2.3.4/24"
    /// const auto ipAddress2 = Ip4Address(string1);
    ///
    /// const auto isEqual = (ipAddress1 == ipAddress2); // true
    /// ```
    [[nodiscard]] String toString() const override;

    /// The length in bytes of an IPv4 subnet address.
    static constexpr uint32_t ADDRESS_LENGTH = Ip4Address::ADDRESS_LENGTH + sizeof(uint8_t);
};

}

#endif
