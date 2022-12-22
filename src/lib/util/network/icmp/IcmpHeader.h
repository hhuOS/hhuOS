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

#ifndef HHUOS_ICMPHEADER_H
#define HHUOS_ICMPHEADER_H

#include <cstdint>

namespace Util {
namespace Stream {
class InputStream;
class OutputStream;
}  // namespace Stream
}  // namespace Util

namespace Util::Network::Icmp {

class IcmpHeader {

public:

    enum Type : uint8_t {
        ECHO_REPLY = 0,
        DESTINATION_UNREACHABLE = 3,
        SOURCE_QUENCH = 4,
        REDIRECT = 5,
        ALTERNATE_HOST_ADDRESS = 6,
        ECHO_REQUEST = 8,
        ROUTER_ADVERTISEMENT = 9,
        ROUTER_SOLICITATION = 10,
        TIME_EXCEEDED = 11,
        BAD_IP_HEADER = 12,
        TIMESTAMP = 13,
        TIMESTAMP_REPLY = 14,
        INFORMATION_REQUEST = 15,
        INFORMATION_REPLY = 16,
        ADDRESS_MASK_REQUEST = 17,
        ADDRESS_MASK_REPLY = 18,
        TRACEROUTE = 30,
        PHOTURIS = 40,
        EXTENDED_ECHO_REQUEST = 42,
        EXTENDED_ECHO_REPLY = 43,
    };

    /**
     * Default Constructor.
     */
    IcmpHeader() = default;

    /**
     * Copy Constructor.
     */
    IcmpHeader(const IcmpHeader &other) = delete;

    /**
     * Assignment operator.
     */
    IcmpHeader &operator=(const IcmpHeader &other) = delete;

    /**
     * Destructor.
     */
    ~IcmpHeader() = default;

    void read(Util::Stream::InputStream &stream);

    void write(Util::Stream::OutputStream &stream);

    [[nodiscard]] Type getType() const;

    void setType(Type type);

    [[nodiscard]] uint8_t getCode() const;

    void setCode(uint8_t code);

    [[nodiscard]] uint16_t getChecksum() const;

    static const constexpr uint32_t CHECKSUM_OFFSET = 2;
    static const constexpr uint32_t HEADER_LENGTH = 4;

private:

    Type type{};
    uint8_t code{};
    uint16_t checksum{};
};

}

#endif
