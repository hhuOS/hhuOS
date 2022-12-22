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

#ifndef HHUOS_IP4HEADER_H
#define HHUOS_IP4HEADER_H

#include <cstdint>

#include "lib/util/network/ip4/Ip4Address.h"

namespace Util {
namespace Stream {
class InputStream;
class OutputStream;
}  // namespace Stream
}  // namespace Util

namespace Util::Network::Ip4 {

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

    void read(Util::Stream::InputStream &stream);

    void write(Util::Stream::OutputStream &stream) const;

    [[nodiscard]] uint8_t getVersion() const;

    [[nodiscard]] uint8_t getHeaderLength() const;

    [[nodiscard]] uint16_t getPayloadLength() const;

    [[nodiscard]] uint8_t getTimeToLive() const;

    [[nodiscard]] Protocol getProtocol() const;

    [[nodiscard]] const Util::Network::Ip4::Ip4Address& getSourceAddress() const;

    [[nodiscard]] const Util::Network::Ip4::Ip4Address& getDestinationAddress() const;

    void setPayloadLength(uint16_t payloadLength);

    void setTimeToLive(uint8_t timeToLive);

    void setProtocol(Protocol aProtocol);

    void setSourceAddress(const Util::Network::Ip4::Ip4Address &sourceAddress);

    void setDestinationAddress(const Util::Network::Ip4::Ip4Address &destinationAddress);

    static const constexpr uint32_t CHECKSUM_OFFSET = 10;

private:

    static const constexpr uint32_t MIN_HEADER_LENGTH = 20;

    uint8_t version = 4;
    uint8_t headerLength = MIN_HEADER_LENGTH;
    uint16_t payloadLength = 0;
    uint8_t timeToLive = 64;
    Protocol protocol{};
    Util::Network::Ip4::Ip4Address sourceAddress{};
    Util::Network::Ip4::Ip4Address destinationAddress{};
};

}

#endif
