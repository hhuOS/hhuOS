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

#include "lib/util/network/icmp/IcmpDatagram.h"

#include "lib/util/network/icmp/IcmpHeader.h"
#include "lib/util/network/ip4/Ip4Address.h"
#include "lib/util/network/NetworkAddress.h"

namespace Util::Network::Icmp {

IcmpDatagram::IcmpDatagram() : Datagram(NetworkAddress::IP4) {}

IcmpDatagram::IcmpDatagram(const uint8_t *buffer, uint16_t length, const Util::Network::Ip4::Ip4Address &remoteAddress, IcmpHeader::Type type, uint8_t code) :
        Datagram(buffer, length, remoteAddress), type(type), code(code) {}

IcmpDatagram::IcmpDatagram(uint8_t *buffer, uint16_t length, const NetworkAddress &remoteAddress, IcmpHeader::Type type, uint8_t code) :
        Datagram(buffer, length, remoteAddress), type(type), code(code) {}

IcmpDatagram::IcmpDatagram(const Stream::ByteArrayOutputStream &stream, const NetworkAddress &remoteAddress, IcmpHeader::Type type, uint8_t code) :
        Datagram(stream, remoteAddress), type(type), code(code) {}

IcmpHeader::Type IcmpDatagram::getType() const {
    return type;
}

uint8_t IcmpDatagram::getCode() const {
    return code;
}

void IcmpDatagram::setAttributes(const Datagram &datagram) {
    auto &icmpDatagram = reinterpret_cast<const IcmpDatagram&>(datagram);
    type = icmpDatagram.getType();
    code = icmpDatagram.getCode();
}

}