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

#include "util/collection/Array.h"
#include "util/network/ip4/Ip4Address.h"

namespace Util::Network::Ip4 {

const Ip4Address Ip4Address::ANY = Ip4Address("0.0.0.0");
const Ip4Address Ip4Address::BROADCAST = Ip4Address("255.255.255.255");

Ip4Address::Ip4Address() : NetworkAddress(ADDRESS_LENGTH, IP4) {}

Ip4Address::Ip4Address(const uint8_t *buffer) : NetworkAddress(buffer, ADDRESS_LENGTH, IP4) {}

Ip4Address::Ip4Address(const Util::String &string) : NetworkAddress(ADDRESS_LENGTH, IP4) {
    auto split = string.split(".");
    const uint8_t buffer[4] = {
            String::parseNumber<uint8_t>(split[0]),
            String::parseNumber<uint8_t>(split[1]),
            String::parseNumber<uint8_t>(split[2]),
            String::parseNumber<uint8_t>(split[3]),
    };

    setAddress(buffer);
}

NetworkAddress* Ip4Address::createCopy() const {
    return new Ip4Address(*this);
}

String Ip4Address::toString() const {
    return String::format("%u.%u.%u.%u", buffer[0], buffer[1], buffer[2], buffer[3]);
}

bool Ip4Address::isBroadcastAddress() const {
    return Address(buffer).compareRange(Address(BROADCAST.buffer), ADDRESS_LENGTH) == 0;
}

}