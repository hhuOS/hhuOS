/*
 * Copyright (C) 2018-2022 Heinrich-Heine-Universitaet Duesseldorf,
 * Institute of Computer Science, Department Operating Systems
 * Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Hannes Feil, Michael Schoettner
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

#ifndef HHUOS_IP4HEADER_H
#define HHUOS_IP4HEADER_H

#include "lib/util/stream/InputStream.h"
#include "Ip4Address.h"

namespace Network::Ip4 {

class Ip4Header {

public:

    enum Protocol : uint8_t {
        ICMP = 0x01,
        TCP = 0x06,
        UDP = 0x11
    };

    /**
     * Default Constructor.
     */
    Ip4Header() = default;

    /**
     * Copy Constructor.
     */
    Ip4Header(const Ip4Header &other) = delete;

    /**
     * Assignment operator.
     */
    Ip4Header &operator=(const Ip4Header &other) = delete;

    /**
     * Destructor.
     */
    ~Ip4Header() = default;

    static uint16_t calculateChecksum(const uint8_t *buffer);

    void read(Util::Stream::InputStream &stream);

    [[nodiscard]] uint8_t getVersion() const;

    [[nodiscard]] uint16_t getPayloadLength() const;

    [[nodiscard]] uint8_t getTimeToLive() const;

    [[nodiscard]] Protocol getProtocol() const;

    [[nodiscard]] Ip4Address getSourceAddress() const;

    [[nodiscard]] Ip4Address getDestinationAddress() const;

private:

    uint8_t version;
    uint16_t payloadLength;
    uint8_t timeToLive;
    Protocol protocol;
    Ip4Address sourceAddress;
    Ip4Address destinationAddress;

    static const constexpr uint32_t MIN_HEADER_LENGTH = 20;
};

}

#endif
