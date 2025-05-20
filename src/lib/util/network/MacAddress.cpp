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

#include "MacAddress.h"

#include "lib/util/network/NetworkAddress.h"
#include "lib/util/collection/Array.h"

namespace Util::Network {

MacAddress::MacAddress() : NetworkAddress(ADDRESS_LENGTH, MAC) {}

MacAddress::MacAddress(const uint8_t *buffer) : NetworkAddress(buffer, ADDRESS_LENGTH, MAC) {}

MacAddress::MacAddress(const Util::String &string) : NetworkAddress(ADDRESS_LENGTH, MAC) {
    auto split = string.split(":");
    uint8_t buffer[6] = {String::parseHexNumber<uint8_t>(split[0]),
                         String::parseHexNumber<uint8_t>(split[1]),
                         String::parseHexNumber<uint8_t>(split[2]),
                         String::parseHexNumber<uint8_t>(split[3]),
                         String::parseHexNumber<uint8_t>(split[4]),
                         String::parseHexNumber<uint8_t>(split[5])};
    NetworkAddress::setAddress(buffer);
}

MacAddress MacAddress::createBroadcastAddress() {
    uint8_t buffer[6] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
    return MacAddress(buffer);
}

bool MacAddress::isBroadcastAddress() const {
    for (uint8_t i = 0; i < ADDRESS_LENGTH; i++) {
        if (buffer[i] != 0xff) return false;
    }

    return true;
}

NetworkAddress *MacAddress::createCopy() const {
    return new MacAddress(*this);
}

Util::String MacAddress::toString() const {
    return Util::String::format("%02x:%02x:%02x:%02x:%02x:%02x", buffer[0], buffer[1], buffer[2], buffer[3], buffer[4], buffer[5]);
}

}