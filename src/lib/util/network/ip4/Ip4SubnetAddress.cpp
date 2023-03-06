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

#include "Ip4SubnetAddress.h"

#include "lib/util/base/Address.h"
#include "lib/util/collection/Array.h"
#include "lib/util/network/ip4/Ip4Address.h"

namespace Util::Network::Ip4 {

Ip4SubnetAddress::Ip4SubnetAddress() : Ip4SubnetAddress(32) {}

Ip4SubnetAddress::Ip4SubnetAddress(uint8_t *buffer) : NetworkAddress(buffer, ADDRESS_LENGTH, IP4_SUBNET) {}

Ip4SubnetAddress::Ip4SubnetAddress(const String &string) : NetworkAddress(ADDRESS_LENGTH, IP4_SUBNET) {
    auto bufferAddress = Util::Address<uint32_t>(buffer);
    auto ip4Address = Ip4Address();
    uint8_t bitCount = 32;

    if (string.beginsWith("/")) {
        bitCount = String::parseInt(string.substring(1));
    } else {
        auto split = string.split("/");
        ip4Address = Ip4Address(split[0]);
        if (split.length() > 1) {
            bitCount = String::parseInt(split[1]);
        }
    }

    ip4Address.getAddress(buffer);
    bufferAddress.setByte(bitCount, Ip4Address::ADDRESS_LENGTH);
}

Ip4SubnetAddress::Ip4SubnetAddress(const Ip4Address &address, uint8_t bitCount) : NetworkAddress(ADDRESS_LENGTH, IP4_SUBNET) {
    uint8_t addressBuffer[Ip4Address::ADDRESS_LENGTH];
    address.getAddress(addressBuffer);

    auto bufferAddress = Address<uint32_t>(buffer);
    bufferAddress.copyRange(Address<uint32_t>(addressBuffer), Ip4Address::ADDRESS_LENGTH);
    bufferAddress.setByte(bitCount, Ip4Address::ADDRESS_LENGTH);
}

Ip4SubnetAddress::Ip4SubnetAddress(const Ip4Address &address) : Ip4SubnetAddress(address, 32) {}

Ip4SubnetAddress::Ip4SubnetAddress(uint8_t bitCount) : Ip4SubnetAddress(Ip4Address::ANY, bitCount) {}

Ip4Address Ip4SubnetAddress::getIp4Address() const {
    return Ip4Address(buffer);
}

uint8_t Ip4SubnetAddress::getBitCount() const {
    return Util::Address<uint32_t>(buffer).getByte(Ip4Address::ADDRESS_LENGTH);
}

Ip4SubnetAddress Ip4SubnetAddress::getSubnetAddress() const {
    // Create bitmask
    auto bitCount = getBitCount();
    uint32_t base = bitCount == 0 ? 0 : 0xffffffff >> (32 - bitCount);
    auto *mask = reinterpret_cast<uint8_t*>(&base);

    // Get address for bitwise and with bitmask
    uint8_t addressBuffer[Util::Network::Ip4::Ip4Address::ADDRESS_LENGTH + 1];
    getIp4Address().getAddress(addressBuffer);

    for (uint8_t i = 0; i < Util::Network::Ip4::Ip4Address::ADDRESS_LENGTH; i++) {
        addressBuffer[i] &= mask[i];
    }

    // Set bit count
    addressBuffer[Util::Network::Ip4::Ip4Address::ADDRESS_LENGTH] = getBitCount();

    return Util::Network::Ip4::Ip4SubnetAddress(addressBuffer);
}

Ip4Address Ip4SubnetAddress::getBroadcastAddress() const {
    // Create bitmask
    uint32_t base = 0xffffffff >> (32 - getBitCount());
    auto *mask = reinterpret_cast<uint8_t*>(&base);

    // Get address for bitwise and with bitmask
    uint8_t addressBuffer[Util::Network::Ip4::Ip4Address::ADDRESS_LENGTH];
    getIp4Address().getAddress(addressBuffer);

    for (uint8_t i = 0; i < Util::Network::Ip4::Ip4Address::ADDRESS_LENGTH; i++) {
        addressBuffer[i] |= mask[i];
    }

    return Util::Network::Ip4::Ip4Address(addressBuffer);
}

NetworkAddress *Ip4SubnetAddress::createCopy() const {
    return new Ip4SubnetAddress(*this);
}

Util::String Ip4SubnetAddress::toString() const {
    return Util::String::format("%u.%u.%u.%u/%u", buffer[0], buffer[1], buffer[2], buffer[3], getBitCount());
}

}