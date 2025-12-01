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

#ifndef HHUOS_LIB_UTIL_NETWORK_UDPHEADER_H
#define HHUOS_LIB_UTIL_NETWORK_UDPHEADER_H

#include <stdint.h>

namespace Util {
namespace Io {
class InputStream;
class OutputStream;
}  // namespace Stream
}  // namespace Util

namespace Util::Network::Udp {

/// Represents a User Datagram Protocol (UDP) header.
/// A UDP header contains the source and destination ports, the length of the datagram, and a checksum.
/// The ports are used to identify the sending and receiving applications on the hosts.
/// The header is 8 bytes long and consists of the following fields:
///
/// | 2 bytes     | 2 bytes          | 2 bytes         | 2 bytes  |
/// |-------------|------------------|-----------------|----------|
/// | Source Port | Destination Port | Datagram Length | Checksum |
class UdpHeader {

public:
    /// Create an empty UDP header.
    /// All fields are initialized to 0.
    UdpHeader() = default;

    /// Read the header values from the given input stream.
    /// The input stream must deliver the 8 bytes of data, that make up the header.
    /// The source port, destination port, datagram length, and checksum are read in this exact order.
    void read(Io::InputStream &stream);

    /// Write the header values to the given output stream.
    /// The output stream will receive the 8 bytes of data, that make up the header.
    /// The source port, destination port, datagram length, and checksum are written in this exact order.
    void write(Io::OutputStream &stream) const;

    /// Get the source port.
    uint16_t getSourcePort() const;

    /// Get the destination port.
    uint16_t getDestinationPort() const;

    /// Get the datagram length in bytes (header + payload).
    uint16_t getDatagramLength() const;

    /// Get the checksum.
    uint16_t getChecksum() const;

    /// Set the source port.
    void setSourcePort(uint16_t sourcePort);

    /// Set the destination port.
    void setDestinationPort(uint16_t destinationPort);

    /// Set the datagram length in bytes (header + payload).
    void setDatagramLength(uint16_t length);

    /// The length of the UDP header in bytes.
    static constexpr uint32_t HEADER_SIZE = 8;

private:

    uint16_t sourcePort = 0;
    uint16_t destinationPort = 0;
    uint16_t datagramLength = 0;
    uint16_t checksum = 0;
};

}

#endif
