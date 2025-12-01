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

#ifndef HHUOS_LIB_UTIL_MACADDRESS_H
#define HHUOS_LIB_UTIL_MACADDRESS_H

#include <stdint.h>

#include "util/base/String.h"
#include "util/network/NetworkAddress.h"

namespace Util::Network {

/// Represents a MAC address, which is a unique identifier for network interfaces.
/// A MAC address is typically six bytes long and tied to a hardware interface.
/// This is a subclass of `NetworkAddress` that provides specific functionality for MAC addresses,
/// such as creating an instance from a string.
class MacAddress final : public NetworkAddress {

public:
    /// Create a new MAC address with a zeroed buffer.
    /// This corresponds to the MAC address `00:00:00:00:00:00`.
    ///
    /// ### Example
    /// ```c++
    /// const auto mac = MacAddress();
    ///
    /// const auto length = mac.getLength(); // 6
    /// const auto string = mac.toString(); // "00:00:00:00:00:00"
    /// const auto isBroadcast = mac.isBroadcastAddress(); // false
    /// ```
    MacAddress();

    /// Create a new MAC address from the given buffer.
    /// The buffer must be at least six bytes long.
    /// If the buffer is shorter, bytes will be read out of bounds, leading to undefined behavior.
    /// If the buffer is longer, only the first six bytes are copied.
    ///
    /// ### Example
    /// ```c++
    /// const uint8_t buffer1[6] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06};
    /// const uint8_t buffer2[8] = {0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e};
    /// const uint8_t buffer3[4] = {0x0F, 0x10, 0x11, 0x12};
    ///
    /// const auto mac1 = MacAddress(buffer1); // 01:02:03:04:05:06
    /// const auto mac2 = MacAddress(buffer2); // 07:08:09:0A:0B:0C
    /// const auto mac3 = MacAddress(buffer3); // Undefined behavior, buffer too short
    /// ```
    explicit MacAddress(const uint8_t *buffer);

    /// Create a new MAC address from a string representation.
    /// The string must be in the format "XX:XX:XX:XX:XX:XX", where each "XX" is a two-digit hexadecimal number.
    /// If the string is not in the correct format, a panic is fired.
    ///
    /// ### Example
    /// ```c++
    /// const auto mac1 = MacAddress("01:02:03:04:05:06");
    /// const auto mac2 = MacAddress("07:08:09:0A:0B:0C");
    /// const auto mac3 = MacAddress("0F:10:11:12"); // Panic: index out of bounds, string too short
    /// ```
    explicit MacAddress(const String &string);

    /// Check if this MAC address is a broadcast address (`ff:ff:ff:ff:ff:ff`).
    ///
    /// ### Example
    /// ```c++
    /// const auto mac1 = MacAddress("01:02:03:04:05:06");
    /// const auto mac2 = MacAddress::BROADCAST;
    /// const auto mac3 = MacAddress("ff:ff:ff:ff:ff:ff");
    ///
    /// const auto isBroadcast1 = mac1.isBroadcastAddress(); // false
    /// const auto isBroadcast2 = mac2.isBroadcastAddress(); // true
    /// const auto isBroadcast3 = mac3.isBroadcastAddress(); // true
    /// ```
    bool isBroadcastAddress() const;

    /// Create a new on-heap instance as a copy of this address.
    ///
    /// ### Example
    /// ```c++
    /// const auto mac1 = MacAddress("01:02:03:04:05:06");
    /// const auto *mac2 = mac1.createCopy();
    ///
    /// const auto isEqual = (mac1 == *mac2); // true
    /// ```
    NetworkAddress* createCopy() const override;

    /// Create a string representation of the MAC address in the format "XX:XX:XX:XX:XX:XX".
    ///
    /// ### Example
    /// ```c++
    /// const auto mac1 = MacAddress("01:02:03:04:05:06");
    /// const auto string1 = mac1.toString(); // "01:02:03:04:05:06"
    /// const auto mac2 = MacAddress(string1);
    ///
    /// const auto isEqual = (mac1 == mac2); // true
    /// ```
    String toString() const override;

    /// A constant storing the broadcast MAC address, which is always `ff:ff:ff:ff:ff:ff`.
    /// The broadcast address is used to send packets to all devices on the local network segment.
    /// It is a special address that is not tied to any specific hardware interface.
    ///
    /// ### Example
    /// ```c++
    /// const auto length = MacAddress::BROADCAST.getLength(); // 6
    /// const auto string = MacAddress::BROADCAST.toString(); // "ff:ff:ff:ff:ff:ff"
    /// const auto isBroadcast = MacAddress::BROADCAST.isBroadcastAddress(); // true
    /// ```
    static const MacAddress BROADCAST;

    /// The length in bytes of a MAC address.
    static constexpr uint8_t ADDRESS_LENGTH = 6;
};

}

#endif
