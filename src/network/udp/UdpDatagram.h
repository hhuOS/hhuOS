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
 */

#ifndef HHUOS_UDPDATAGRAM_H
#define HHUOS_UDPDATAGRAM_H

#include <cstdint>
#include "network/NetworkAddress.h"

namespace Network::Udp {

class UdpDatagram {

public:
    /**
     * Default Constructor.
     */
    UdpDatagram() = default;

    /**
     * Constructor.
     */
    UdpDatagram(const uint8_t *buffer, uint16_t length, const Network::NetworkAddress &remoteAddress, uint16_t remotePort);

    /**
     * Copy Constructor.
     */
    UdpDatagram(const UdpDatagram &other) = default;

    /**
     * Assignment operator.
     */
    UdpDatagram &operator=(const UdpDatagram &other) = default;

    /**
     * Destructor.
     */
    ~UdpDatagram();

    bool operator!=(const UdpDatagram &other);

    [[nodiscard]] uint16_t getLength() const;

    [[nodiscard]] const uint8_t* getBuffer() const;

    [[nodiscard]] const Network::NetworkAddress& getRemoteAddress() const;

    [[nodiscard]] uint16_t getRemotePort() const;

private:

    const uint8_t *buffer{};
    uint16_t length{};

    const Network::NetworkAddress *remoteAddress;
    uint16_t remotePort;
};

}

#endif
