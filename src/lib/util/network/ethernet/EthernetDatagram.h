/*
 * Copyright (C) 2018-2025 Heinrich-Heine-Universitaet Duesseldorf,
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
 * The UDP/IP stack is based on a bachelor's thesis, written by Hannes Feil.
 * The original source code can be found here: https://github.com/hhuOS/hhuOS/tree/legacy/network
 */

#ifndef HHUOS_ETHERNETDATAGRAM_H
#define HHUOS_ETHERNETDATAGRAM_H

#include <stdint.h>

#include "lib/util/network/ethernet/EthernetHeader.h"
#include "lib/util/network/Datagram.h"

namespace Util {
namespace Io {
class ByteArrayOutputStream;
}  // namespace Io

namespace Network {
class MacAddress;
class NetworkAddress;
}  // namespace Network
}  // namespace Util

namespace Util::Network::Ethernet {

class EthernetDatagram : public Datagram {

public:
    /**
     * Default Constructor.
     */
    EthernetDatagram();

    /**
     * Constructor.
     */
    EthernetDatagram(const uint8_t *buffer, uint16_t length, const Util::Network::MacAddress &remoteAddress, EthernetHeader::EtherType type);

    /**
     * Constructor.
     */
    EthernetDatagram(uint8_t *buffer, uint16_t length, const Util::Network::NetworkAddress &remoteAddress, EthernetHeader::EtherType type);

    /**
     * Constructor.
     */
    EthernetDatagram(const Io::ByteArrayOutputStream &stream, const Util::Network::NetworkAddress &remoteAddress, EthernetHeader::EtherType type);

    /**
     * Copy Constructor.
     */
    EthernetDatagram(const EthernetDatagram &other) = delete;

    /**
     * Assignment operator.
     */
    EthernetDatagram &operator=(const EthernetDatagram &other) = delete;

    /**
     * Destructor.
     */
    ~EthernetDatagram() override = default;

    [[nodiscard]] EthernetHeader::EtherType getEtherType() const;

    void setAttributes(const Datagram &datagram) override;

private:

    EthernetHeader::EtherType type{};
};

}

#endif
