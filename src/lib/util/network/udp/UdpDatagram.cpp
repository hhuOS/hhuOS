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

#include "UdpDatagram.h"

#include "lib/util/network/ip4/Ip4PortAddress.h"
#include "lib/util/network/NetworkAddress.h"

namespace Util::Network::Udp {

UdpDatagram::UdpDatagram() : Datagram(NetworkAddress::IP4_PORT) {}

UdpDatagram::UdpDatagram(const uint8_t *buffer, uint16_t length, const Util::Network::Ip4::Ip4PortAddress &remoteAddress)
        : Datagram(buffer, length, remoteAddress) {}

UdpDatagram::UdpDatagram(uint8_t *buffer, uint16_t length, const NetworkAddress &remoteAddress)
        : Datagram(buffer, length, remoteAddress) {}

UdpDatagram::UdpDatagram(const Stream::ByteArrayOutputStream &stream, const NetworkAddress &remoteAddress)
        : Datagram(stream, remoteAddress) {}

uint16_t UdpDatagram::getRemotePort() const {
    return reinterpret_cast<const Util::Network::Ip4::Ip4PortAddress*>(remoteAddress)->getPort();
}

void UdpDatagram::setAttributes(const Datagram &datagram) {}

}