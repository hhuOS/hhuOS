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

#ifndef HHUOS_UDPDATAGRAM_H
#define HHUOS_UDPDATAGRAM_H

#include <cstdint>

#include "lib/util/network/Datagram.h"

namespace Util {
namespace Network {
namespace Ip4 {
class Ip4PortAddress;
}  // namespace Ip4
}  // namespace Network
}  // namespace Util

namespace Util::Network::Udp {

class UdpDatagram : public Datagram {

public:
    /**
     * Default Constructor.
     */
    UdpDatagram();

    /**
     * Constructor.
     */
    UdpDatagram(const uint8_t *buffer, uint16_t length, const Util::Network::Ip4::Ip4PortAddress &remoteAddress);

    /**
     * Constructor.
     */
    UdpDatagram(uint8_t *buffer, uint16_t length, const Util::Network::NetworkAddress &remoteAddress);

    /**
     * Constructor.
     */
    UdpDatagram(const Stream::ByteArrayOutputStream &stream, const Util::Network::NetworkAddress &remoteAddress);

    /**
     * Copy Constructor.
     */
    UdpDatagram(const UdpDatagram &other) = delete;

    /**
     * Assignment operator.
     */
    UdpDatagram &operator=(const UdpDatagram &other) = delete;

    /**
     * Destructor.
     */
    ~UdpDatagram() override = default;

    [[nodiscard]] uint16_t getRemotePort() const;

    void setAttributes(const Datagram &datagram) override;
};

}

#endif
