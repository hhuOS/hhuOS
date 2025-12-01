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

#ifndef HHUOS_LIB_UTIL_NETWORK_ECHOHEADER_H
#define HHUOS_LIB_UTIL_NETWORK_ECHOHEADER_H

#include <stdint.h>

namespace Util {
namespace Io {
class InputStream;
class OutputStream;
}  // namespace Stream
}  // namespace Util

namespace Util::Network::Icmp {

/// Represents a header for an Echo Request or Echo Reply in the Internet Control Message Protocol (ICMP).
/// The Echo message is contained in the payload of an ICMP packet
/// and the header follows directly after the ICMP header.
/// The header is 4 bytes long and consists of the following fields:
///
/// | 2 bytes    | 2 bytes         |
/// |------------|-----------------|
/// | Identifier | Sequence Number |
class EchoHeader {

public:
    /// Create an empty Echo header.
    /// The identifier and sequence number are initialized to 0.
    EchoHeader() = default;

    /// Read the header values from the given input stream.
    /// The input stream must deliver the 4 bytes of data, that make up the header.
    /// The identifier and sequence number are read in this exact order.
    void read(Io::InputStream &stream);

    /// Write the header values to the given output stream.
    /// The output stream will receive the 4 bytes of data, that make up the header.
    /// The identifier and sequence number are written in this exact order.
    void write(Io::OutputStream &stream) const;

    /// Get the identifier.
    uint16_t getIdentifier() const;

    /// Get the sequence number.
    uint16_t getSequenceNumber() const;

    /// Set the identifier.
    void setIdentifier(uint16_t identifier);

    /// Set the sequence number.
    void setSequenceNumber(uint16_t sequenceNumber);

    /// The length of the Echo header in bytes.
    static constexpr uint32_t HEADER_LENGTH = 4;

private:

    uint16_t identifier = 0;
    uint16_t sequenceNumber = 0;
};

}

#endif
