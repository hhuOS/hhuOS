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
 *
 * The network stack is based on a bachelor's thesis, written by Hannes Feil.
 * The original source code can be found here: https://github.com/hhuOS/hhuOS/tree/legacy/network
 */

#include "lib/util/network/ip4/Ip4Address.h"

#include "lib/util/Exception.h"
#include "lib/util/data/Array.h"

namespace Util::Network::Ip4 {

const Ip4Address Ip4Address::ANY = Ip4Address("0.0.0.0");

Ip4Address::Ip4Address() : NetworkAddress(ADDRESS_LENGTH, IP4) {}

Ip4Address::Ip4Address(uint8_t *buffer) : NetworkAddress(buffer, ADDRESS_LENGTH, IP4) {}

Ip4Address::Ip4Address(const Util::Memory::String &string) : NetworkAddress(ADDRESS_LENGTH, IP4) {
    auto split = string.split(".");
    uint8_t buffer[4] = {
            static_cast<uint8_t>(Util::Memory::String::parseInt(split[0])),
            static_cast<uint8_t>(Util::Memory::String::parseInt(split[1])),
            static_cast<uint8_t>(Util::Memory::String::parseInt(split[2])),
            static_cast<uint8_t>(Util::Memory::String::parseInt(split[3])),
    };
    NetworkAddress::setAddress(buffer);
}

NetworkAddress* Ip4Address::createCopy() const {
    return new Ip4Address(*this);
}

void Ip4Address::setAddress(const Util::Memory::String &string) {
    auto split = string.split(".");
    if (split.length() != 4) {
        Util::Exception::throwException(Util::Exception::INVALID_ARGUMENT, "Ip4Address: Invalid address string given!");
    }

    for (uint8_t i = 0; i < ADDRESS_LENGTH; i++) {
        buffer[i] = Util::Memory::String::parseInt(split[i]);
    }
}

Util::Memory::String Ip4Address::toString() const {
    return Util::Memory::String::format("%u.%u.%u.%u", buffer[0], buffer[1], buffer[2], buffer[3]);
}

Ip4Address Ip4Address::createBroadcastAddress() {
    uint8_t buffer[4] = {0xff, 0xff, 0xff, 0xff};
    return Ip4Address(buffer);
}

bool Ip4Address::isBroadcastAddress() const {
    for (uint8_t i = 0; i < ADDRESS_LENGTH; i++) {
        if (buffer[i] != 0xff) return false;
    }

    return true;
}

}