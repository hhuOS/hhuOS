/*
 * Copyright (C) 2017-2026 Heinrich Heine University Düsseldorf,
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

#ifndef HHUOS_LIB_UTIL_NETWORK_ICMPHEADER_H
#define HHUOS_LIB_UTIL_NETWORK_ICMPHEADER_H

#include <stdint.h>

#include "util/io/stream/InputStream.h"
#include "util/io/stream/OutputStream.h"

namespace Util {
namespace Network {
namespace Icmp {

/// Represents an Internet Control Message Protocol (ICMP) header,
/// which is used for error messages and operational information in network communication.
/// For example, ICMP is used for ping requests and replies.
/// The header is 4 bytes long and consists of the following fields:
///
/// | 1 byte | 1 byte | 2 bytes   |
/// |--------|--------|-----------|
/// | Type   | Code   | Checksum  |
class IcmpHeader {

public:
    /// Different types of ICMP messages (full list is available in RFC792).
    enum Type : uint8_t {
        /// Echo Reply, used in response to an Echo Request (ping).
        ECHO_REPLY = 0,
        /// Destination Unreachable, sent when a destination cannot be reached.
        DESTINATION_UNREACHABLE = 3,
        /// Redirect, used to inform a host of a better route to a destination.
        REDIRECT = 5,
        /// Echo Request, used to request an Echo Reply (ping).
        ECHO_REQUEST = 8,
        /// Router Advertisement, used by routers to advertise their presence.
        ROUTER_ADVERTISEMENT = 9,
        /// Router Solicitation, used by hosts to request router advertisements.
        ROUTER_SOLICITATION = 10,
        /// Time Exceeded, sent when a packet's time-to-live (TTL) expires.
        TIME_EXCEEDED = 11,
        /// Parameter Problem, sent when there is a problem with the IP header of a packet.
        BAD_IP_HEADER = 12,
        /// Timestamp Request, used to request a timestamp reply.
        TIMESTAMP = 13,
        /// Timestamp Reply, used in response to a Timestamp Request.
        TIMESTAMP_REPLY = 14
    };

    /// Create an empty ICMP header.
    /// The type is set to `ECHO_REPLY`, code is set to `0`, and checksum is set to `0`.
    IcmpHeader() = default;

    /// Read the header values from the given input stream.
    /// The input stream must deliver the 4 bytes of data, that make up the ICMP header.
    /// The type, code, and checksum fields are read in this exact order.
    void read(Io::InputStream &stream);

    /// Write the header values to the given output stream.
    /// The output stream will receive the 4 bytes of data, that make up the ICMP header.
    /// The type, code, and checksum fields are written in this exact order.
    void write(Io::OutputStream &stream) const;

    /// Get the ICMP type.
    Type getType() const {
        return type;
    }

    /// Get the code.
    uint8_t getCode() const {
        return code;
    }

    /// Get the checksum.
    uint16_t getChecksum() const {
        return checksum;
    }

    /// Set the ICMP type.
    void setType(Type type) {
        IcmpHeader::type = type;
    }

    /// Set the code.
    void setCode(uint8_t code) {
        IcmpHeader::code = code;
    }

    /// The offset of the checksum field in the ICMP header.
    static constexpr uint32_t CHECKSUM_OFFSET = 2;
    /// The length of the ICMP header in bytes.
    static constexpr uint32_t HEADER_LENGTH = 4;

private:

    Type type = ECHO_REPLY;
    uint8_t code = 0;
    uint16_t checksum = 0;
};

}
}
}

#endif
