/*
 * Copyright (C) 2018-2025 Heinrich-Heine-Universitaet Duesseldorf,
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
 *
 * The UDP/IP stack is based on a bachelor's thesis, written by Hannes Feil.
 * The original source code can be found here: https://github.com/hhuOS/hhuOS/tree/legacy/network
 */

#include "Ip4PortAddress.h"

#include "lib/util/collection/Array.h"
#include "lib/util/network/ip4/Ip4Address.h"
#include "lib/util/base/Address.h"

namespace Util::Network::Ip4 {

Ip4PortAddress::Ip4PortAddress() : NetworkAddress(ADDRESS_LENGTH, IP4_PORT) {}

Ip4PortAddress::Ip4PortAddress(uint8_t *buffer) : NetworkAddress(buffer, ADDRESS_LENGTH, IP4_PORT) {}

Ip4PortAddress::Ip4PortAddress(const String &string) : NetworkAddress(ADDRESS_LENGTH, IP4_PORT) {
    auto bufferAddress = Util::Address(buffer);
    auto ip4Address = Ip4Address();
    uint16_t port = 0;

    if (string.beginsWith(":")) {
        port = String::parseInt(string.substring(1));
    } else {
        auto split = string.split(":");
        ip4Address = Ip4Address(split[0]);
        if (split.length() > 1) {
            port = String::parseInt(split[1]);
        }
    }

    ip4Address.getAddress(buffer);
    bufferAddress.setShort(port, Ip4Address::ADDRESS_LENGTH);
}

Ip4PortAddress::Ip4PortAddress(const Ip4Address &address, uint16_t port) : NetworkAddress(Ip4Address::ADDRESS_LENGTH + 2, IP4_PORT) {
    uint8_t addressBuffer[Ip4Address::ADDRESS_LENGTH];
    address.getAddress(addressBuffer);

    auto bufferAddress = Address(buffer);
    bufferAddress.copyRange(Address(addressBuffer), Ip4Address::ADDRESS_LENGTH);
    bufferAddress.setShort(port, Ip4Address::ADDRESS_LENGTH);
}

Ip4PortAddress::Ip4PortAddress(const Ip4Address &address) : Ip4PortAddress(address, 0) {}

Ip4PortAddress::Ip4PortAddress(uint16_t port) : Ip4PortAddress(Ip4Address::ANY, port) {}

Ip4Address Ip4PortAddress::getIp4Address() const {
    return Ip4Address(buffer);
}

uint16_t Ip4PortAddress::getPort() const {
    return Util::Address(buffer).getShort(Ip4Address::ADDRESS_LENGTH);
}

void Ip4PortAddress::setPort(uint16_t port) {
    Util::Address(buffer).setShort(port, Ip4Address::ADDRESS_LENGTH);
}

NetworkAddress* Ip4PortAddress::createCopy() const {
    return new Ip4PortAddress(*this);
}

Util::String Ip4PortAddress::toString() const {
    return Util::String::format("%u.%u.%u.%u:%u", buffer[0], buffer[1], buffer[2], buffer[3], getPort());
}

}