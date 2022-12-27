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

#include "lib/util/network/ethernet/EthernetDatagram.h"

#include "lib/util/network/MacAddress.h"
#include "lib/util/network/ethernet/EthernetHeader.h"
#include "lib/util/network/NetworkAddress.h"

namespace Util::Network::Ethernet {

EthernetDatagram::EthernetDatagram() : Datagram(NetworkAddress::MAC) {}

EthernetDatagram::EthernetDatagram(const uint8_t *buffer, uint16_t length, const Util::Network::MacAddress &remoteAddress, Network::Ethernet::EthernetHeader::EtherType type) :
        Datagram(buffer, length, remoteAddress), type(type) {}

EthernetDatagram::EthernetDatagram(uint8_t *buffer, uint16_t length, const NetworkAddress &remoteAddress, EthernetHeader::EtherType type) :
        Datagram(buffer, length, remoteAddress), type(type) {}

EthernetDatagram::EthernetDatagram(const Stream::ByteArrayOutputStream &stream, const NetworkAddress &remoteAddress, EthernetHeader::EtherType type) :
        Datagram(stream, remoteAddress), type(type) {}

EthernetHeader::EtherType Network::Ethernet::EthernetDatagram::getEtherType() const {
    return type;
}

void EthernetDatagram::setAttributes(const Datagram &datagram) {
    auto &ethernetDatagram = reinterpret_cast<const EthernetDatagram&>(datagram);
    type = ethernetDatagram.getEtherType();
}

}