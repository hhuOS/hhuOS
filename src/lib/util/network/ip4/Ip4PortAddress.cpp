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

#include "Ip4PortAddress.h"

#include "base/Address.h"
#include "collection/Array.h"
#include "network/ip4/Ip4Address.h"

namespace Util::Network::Ip4 {

Ip4PortAddress::Ip4PortAddress() : NetworkAddress(ADDRESS_LENGTH, IP4_PORT) {}

Ip4PortAddress::Ip4PortAddress(const uint8_t *buffer) : NetworkAddress(buffer, ADDRESS_LENGTH, IP4_PORT) {}

Ip4PortAddress::Ip4PortAddress(const String &string) : NetworkAddress(ADDRESS_LENGTH, IP4_PORT) {
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

Ip4PortAddress::Ip4PortAddress(const Ip4Address &address, const uint16_t port) :
        NetworkAddress(Ip4Address::ADDRESS_LENGTH + 2, IP4_PORT) {
    // Copy the IP address to the buffer and write the port number
    address.getAddress(buffer);
    Address(buffer).write16(port, Ip4Address::ADDRESS_LENGTH);
}

Ip4PortAddress::Ip4PortAddress(const Ip4Address &address) : NetworkAddress(ADDRESS_LENGTH, IP4_PORT) {
    // Copy the IP address to the buffer (port is already initialized to 0)
    address.getAddress(buffer);
}

Ip4PortAddress::Ip4PortAddress(const uint16_t port) : NetworkAddress(ADDRESS_LENGTH, IP4_PORT) {
    // Buffer is already initialized with "0.0.0.0:0" -> Just set the port number
    Address(buffer).write16(port, Ip4Address::ADDRESS_LENGTH);
}

Ip4Address Ip4PortAddress::getIp4Address() const {
    return Ip4Address(buffer);
}

uint16_t Ip4PortAddress::getPort() const {
    return Address(buffer).read16(Ip4Address::ADDRESS_LENGTH);
}

void Ip4PortAddress::setPort(const uint16_t port) const {
    Address(buffer).write16(port, Ip4Address::ADDRESS_LENGTH);
}

NetworkAddress* Ip4PortAddress::createCopy() const {
    return new Ip4PortAddress(*this);
}

String Ip4PortAddress::toString() const {
    return String::format("%u.%u.%u.%u:%u", buffer[0], buffer[1], buffer[2], buffer[3], getPort());
}

}