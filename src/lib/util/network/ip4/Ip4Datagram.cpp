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

#include "Ip4Datagram.h"

#include "network/ip4/Ip4Address.h"
#include "network/ip4/Ip4Header.h"
#include "network/NetworkAddress.h"

namespace Util {
namespace Io {
class ByteArrayOutputStream;
}  // namespace Io
}  // namespace Util

namespace Util::Network::Ip4 {

Ip4Datagram::Ip4Datagram() : Datagram(NetworkAddress::IP4) {}

Ip4Datagram::Ip4Datagram(const uint8_t *buffer, const uint16_t length, const Ip4Address &remoteAddress,
        const Ip4Header::Protocol protocol) : Datagram(buffer, length, remoteAddress), protocol(protocol) {}

Ip4Datagram::Ip4Datagram(const Io::ByteArrayOutputStream &stream, const NetworkAddress &remoteAddress,
        const Ip4Header::Protocol protocol) : Datagram(stream, remoteAddress), protocol(protocol) {}

Ip4Header::Protocol Ip4Datagram::getProtocol() const {
    return protocol;
}

void Ip4Datagram::setAttributes(const Datagram &datagram) {
    const auto &ip4Datagram = reinterpret_cast<const Ip4Datagram&>(datagram);
    protocol = ip4Datagram.getProtocol();
}

}