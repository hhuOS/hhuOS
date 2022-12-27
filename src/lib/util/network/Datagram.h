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

#ifndef HHUOS_DATAGRAM_H
#define HHUOS_DATAGRAM_H

#include <cstdint>

#include "NetworkAddress.h"
#include "lib/util/stream/ByteArrayOutputStream.h"

namespace Util::Network {

class Datagram {

public:
    /**
     * Constructor.
     */
    explicit Datagram(NetworkAddress::Type type);

    /**
     * Constructor.
     */
    Datagram(const uint8_t *buffer, uint16_t length, const Util::Network::NetworkAddress &remoteAddress);

    /**
     * Constructor.
     */
    Datagram(uint8_t *buffer, uint16_t length, const Util::Network::NetworkAddress &remoteAddress);

    /**
     * Constructor.
     */
    Datagram(const Stream::ByteArrayOutputStream &stream, const Util::Network::NetworkAddress &remoteAddress);

    /**
     * Copy Constructor.
     */
    Datagram(const Datagram &other) = delete;

    /**
     * Assignment operator.
     */
    Datagram &operator=(const Datagram &other) = delete;

    /**
     * Destructor.
     */
    virtual ~Datagram();

    [[nodiscard]] const Util::Network::NetworkAddress& getRemoteAddress() const;

    void setRemoteAddress(const Util::Network::NetworkAddress& address);

    [[nodiscard]] uint8_t* getData() const;

    [[nodiscard]] uint32_t getLength() const;

    void setData(uint8_t *buffer, uint32_t length);

    virtual void setAttributes(const Datagram &datagram) = 0;

protected:

    Util::Network::NetworkAddress *remoteAddress{};

    uint8_t *buffer{};
    uint32_t length{};
};

}

#endif
