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

#ifndef HHUOS_IP4DATAGRAM_H
#define HHUOS_IP4DATAGRAM_H

#include "Ip4Address.h"
#include "network/Datagram.h"
#include "Ip4Header.h"

namespace Network::Ip4 {

class Ip4Datagram : public Datagram {

public:
    /**
     * Constructor.
     */
    Ip4Datagram(const uint8_t *buffer, uint16_t length, const Ip4::Ip4Address &remoteAddress, Ip4Header::Protocol protocol);

    /**
     * Copy Constructor.
     */
    Ip4Datagram(const Ip4Datagram &other) = delete;

    /**
     * Assignment operator.
     */
    Ip4Datagram &operator=(const Ip4Datagram &other) = delete;

    /**
     * Destructor.
     */
    ~Ip4Datagram() = default;

    [[nodiscard]] Ip4Header::Protocol getProtocol() const;

private:

    Ip4Header::Protocol protocol;
};

}

#endif
