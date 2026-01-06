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

#include "Ip4SubnetAddress.h"

#include "util/base/Address.h"
#include "util/collection/Array.h"
#include "util/network/ip4/Ip4Address.h"

namespace Util {
namespace Network {
namespace Ip4 {

Ip4SubnetAddress::Ip4SubnetAddress(const String &string) : NetworkAddress(ADDRESS_LENGTH, IP4_SUBNET) {
    const auto bufferAddress = Address(buffer);

    if (string.beginsWith("/")) {
        const auto bitCount = String::parseNumber<uint8_t>(string.substring(1));
        if (bitCount > 32) {
            Panic::fire(Panic::INVALID_ARGUMENT, "Ip4SubnetAddress: Bit count must be between 0 and 32!");
        }

        // Buffer is already initialized with "0.0.0.0/0" -> Just set the bit count
        bufferAddress.write8(bitCount, Ip4Address::ADDRESS_LENGTH);
    } else {
        const auto split = string.split("/");
        const auto ip4Address = Ip4Address(split[0]);
        const auto bitCount = split.length() > 1 ? String::parseNumber<uint8_t>(split[1]) : 32;
        if (bitCount > 32) {
            Panic::fire(Panic::INVALID_ARGUMENT, "Ip4SubnetAddress: Bit count must be between 0 and 32!");
        }

        // Write the IP address and bit count to the buffer
        ip4Address.getAddress(buffer);
        bufferAddress.write8(bitCount, Ip4Address::ADDRESS_LENGTH);
    }
}

Ip4SubnetAddress Ip4SubnetAddress::getSubnetAddress() const {
    // Create bitmask
    const auto bitCount = getBitCount();
    const auto base = bitCount == 0 ? 0 : 0xffffffff >> (32 - bitCount);
    const auto *mask = reinterpret_cast<const uint8_t*>(&base);

    // Get address for bitwise and with bitmask
    uint8_t addressBuffer[Ip4Address::ADDRESS_LENGTH + 1];
    getIp4Address().getAddress(addressBuffer);

    for (uint8_t i = 0; i < Ip4Address::ADDRESS_LENGTH; i++) {
        addressBuffer[i] &= mask[i];
    }

    // Set bit count
    addressBuffer[Ip4Address::ADDRESS_LENGTH] = getBitCount();

    return Ip4SubnetAddress(addressBuffer);
}

Ip4Address Ip4SubnetAddress::getBroadcastAddress() const {
    // Create bitmask
    const auto base = 0xffffffff >> (32 - getBitCount());
    const auto *mask = reinterpret_cast<const uint8_t*>(&base);

    // Get address for bitwise or with bitmask
    uint8_t addressBuffer[Ip4Address::ADDRESS_LENGTH];
    getIp4Address().getAddress(addressBuffer);

    for (uint8_t i = 0; i < Ip4Address::ADDRESS_LENGTH; i++) {
        addressBuffer[i] |= mask[i];
    }

    return Ip4Address(addressBuffer);
}

}
}
}