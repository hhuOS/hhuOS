/*
 * Copyright (C) 2018-2022 Heinrich-Heine-Universitaet Duesseldorf,
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

#include "Ip4PortAddress.h"

#include "lib/util/Exception.h"
#include "lib/util/data/Array.h"
#include "network/ip4/Ip4Address.h"

namespace Network::Ip4 {

Ip4PortAddress::Ip4PortAddress(const Ip4Address &address, uint16_t port) : NetworkAddress(Ip4Address::ADDRESS_LENGTH + 2, IP4_PORT), bufferAddress(buffer) {
    uint8_t addressBuffer[Ip4Address::ADDRESS_LENGTH];
    address.getAddress(addressBuffer);
    bufferAddress.copyRange(Util::Memory::Address<uint32_t>(addressBuffer), Ip4Address::ADDRESS_LENGTH);
    bufferAddress.setShort(port, Ip4Address::ADDRESS_LENGTH);
}

Ip4Address Ip4PortAddress::getIp4Address() const {
    return Ip4Address(buffer);
}

uint16_t Ip4PortAddress::getPort() const {
    return bufferAddress.getShort(Ip4Address::ADDRESS_LENGTH);
}

NetworkAddress* Ip4PortAddress::createCopy() const {
    return new Ip4PortAddress(*this);
}

void Ip4PortAddress::setAddress(const Util::Memory::String &string) {
    auto split = string.split(":");
    if (split.length() != 2) {
        Util::Exception::throwException(Util::Exception::INVALID_ARGUMENT, "Ip4SocketAddress: Invalid address string given!");
    }

    uint16_t port = Util::Memory::String::parseInt(split[1]);

    split = split[0].split(".");
    if (split.length() != 4) {
        Util::Exception::throwException(Util::Exception::INVALID_ARGUMENT, "Ip4Address: Invalid address string given!");
    }

    for (uint8_t i = 0; i < Ip4Address::ADDRESS_LENGTH; i++) {
        buffer[i] = Util::Memory::String::parseInt(split[i]);
    }

    bufferAddress.setShort(port, Ip4Address::ADDRESS_LENGTH);
}

Util::Memory::String Ip4PortAddress::toString() const {
    return Util::Memory::String::format("%u.%u.%u.%u:%d", buffer[0], buffer[1], buffer[2], buffer[3], getPort());
}

}