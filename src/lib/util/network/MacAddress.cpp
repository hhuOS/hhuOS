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

#include "MacAddress.h"

#include "lib/util/Exception.h"
#include "lib/util/network/NetworkAddress.h"

namespace Util::Network {

MacAddress::MacAddress() : NetworkAddress(ADDRESS_LENGTH, MAC) {}

MacAddress::MacAddress(const uint8_t *buffer) : NetworkAddress(buffer, ADDRESS_LENGTH, MAC) {}

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

Util::Memory::String MacAddress::toString() const {
    return Util::Memory::String::format("%02x:%02x:%02x:%02x:%02x:%02x", buffer[0], buffer[1], buffer[2], buffer[3], buffer[4], buffer[5]);
}

void MacAddress::setAddress(const Util::Memory::String &string) {
    Util::Exception::throwException(Util::Exception::UNSUPPORTED_OPERATION, "MacAddress: Cannot be parsed from string!");
}

}