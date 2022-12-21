/*
 * Copyright (C) 2018-2022 Heinrich-Heine-Universitaet Duesseldorf,
 * Institute of Computer Science, Department Operating Systems
 * Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Hannes Feil, Michael Schoettner
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

#include "Ip4NetworkMask.h"

#include "lib/util/network/ip4/Ip4Address.h"

namespace Network::Ip4 {

Ip4NetworkMask::Ip4NetworkMask() : NetworkMask(32) {}

Ip4NetworkMask::Ip4NetworkMask(uint8_t bitCount) : NetworkMask(bitCount) {}

bool Ip4NetworkMask::operator==(const Ip4NetworkMask &other) const {
    return bitCount == other.bitCount;
}

bool Ip4NetworkMask::operator!=(const Ip4NetworkMask &other) const {
    return bitCount != other.bitCount;
}

Util::Network::Ip4::Ip4Address Ip4NetworkMask::extractSubnet(const Util::Network::Ip4::Ip4Address &address) const {
    // Create bitmask
    uint32_t base = 0xffffffff << (32 - bitCount);
    auto *mask = reinterpret_cast<uint8_t*>(&base);

    // Get address for bitwise and with bitmask
    uint8_t addressBuffer[4];
    address.getAddress(addressBuffer);

    for (uint8_t i = 0; i < Util::Network::Ip4::Ip4Address::ADDRESS_LENGTH; i++) {
        addressBuffer[i] &= mask[i];
    }

    return Util::Network::Ip4::Ip4Address(addressBuffer);
}

Util::Network::Ip4::Ip4Address Ip4NetworkMask::createBroadcastAddress(const Util::Network::Ip4::Ip4Address &networkAddress) const {
    // Create bitmask
    uint32_t base = 0xffffffff >> (32 - bitCount);
    auto *mask = reinterpret_cast<uint8_t*>(&base);

    // Get address for bitwise and with bitmask
    uint8_t addressBuffer[4];
    networkAddress.getAddress(addressBuffer);

    for (uint8_t i = 0; i < Util::Network::Ip4::Ip4Address::ADDRESS_LENGTH; i++) {
        addressBuffer[i] |= mask[i];
    }

    return Util::Network::Ip4::Ip4Address(addressBuffer);
}

}