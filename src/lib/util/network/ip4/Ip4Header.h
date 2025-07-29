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

#ifndef HHUOS_LIB_UTIL_NETWORK_IP4HEADER_H
#define HHUOS_LIB_UTIL_NETWORK_IP4HEADER_H

#include <stdint.h>

#include "network/ip4/Ip4Address.h"

namespace Util {
namespace Io {
class InputStream;
class OutputStream;
}  // namespace Stream
}  // namespace Util

namespace Util::Network::Ip4 {

/// Represents the header of an IPv4 packet.
/// An IPv4 header contains, among other things, the version, header length, total packet length
/// source and destination IP addresses, time to live, and protocol.
/// The header is at least 20 bytes long and consists of the following fields:
///
/// | 1 byte          | 1 byte      | 2 bytes      | 2 bytes        | 2 bytes        | 1 b | 1 byte   | 2 bytes  |
/// |-----------------|-------------|--------------|----------------|----------------|-----|----------|----------|
/// | Version + Length | DSCP + ECN | Total Length | Identification | Flags + Offset | TTL | Protocol | Checksum |
///
/// | 4 bytes        | 4 bytes             |
/// |----------------|---------------------|
/// | Source Address | Destination Address |
class Ip4Header {

public:
    /// Different protocols that can be carried in an IPv4 packet.
    enum Protocol : uint8_t {
        /// ICMP (Internet Control Message Protocol)
        ICMP = 0x01,
        /// TCP (Transmission Control Protocol)
        TCP = 0x06,
        /// UDP (User Datagram Protocol)
        UDP = 0x11,
        /// Used to indicate that the protocol field is invalid.
        INVALID = 0xff
    };

    /// Create a new empty IPv4 header.
    /// Version is set to 4, header length to 20 bytes (minimum), payload length to 0, and time to live to 64.
    /// The protocol is set to `INVALID`, and both source and destination addresses are initialized to `0.0.0.0`.
    Ip4Header() = default;

    /// Read the header values from the given input stream.
    /// The input stream must deliver at least 20 bytes of data, that make up the IPv4 header.
    /// The version, header length, DSCP, ECN, total length, identification, flags, offset,
    /// time to live, protocol, checksum, source address, and destination address are read in this exact order.
    void read(Io::InputStream &stream);

    /// Write the header values to the given output stream.
    /// The output stream will receive the 20 bytes of data, that make up the IPv4 header.
    /// The version, header length, DSCP, ECN, total length, identification, flags, offset,
    /// time to live, protocol, checksum, source address, and destination address are written in this exact order.
    void write(Io::OutputStream &stream) const;

    /// Get the IP version (should always be 4 for IPv4).
    [[nodiscard]] uint8_t getVersion() const;

    /// Get the header length in bytes.
    [[nodiscard]] uint8_t getHeaderLength() const;

    /// Get the payload length in bytes.
    /// That is the length of the data following the header (total length minus header length).
    [[nodiscard]] uint16_t getPayloadLength() const;

    /// Get the time to live (TTL) value.
    [[nodiscard]] uint8_t getTimeToLive() const;

    /// Get the protocol carried in this IPv4 packet (e.g., ICMP, TCP, UDP).
    [[nodiscard]] Protocol getProtocol() const;

    /// Get the IP address of the sender (source address).
    [[nodiscard]] const Ip4Address& getSourceAddress() const;

    /// Get the IP address of the receiver (destination address).
    [[nodiscard]] const Ip4Address& getDestinationAddress() const;

    /// Set the header length in bytes.
    void setPayloadLength(uint16_t payloadLength);

    /// Set the time to live (TTL) value.
    void setTimeToLive(uint8_t timeToLive);

    /// Set the protocol carried in this IPv4 packet.
    void setProtocol(Protocol protocol);

    /// Set the source IP address.
    void setSourceAddress(const Ip4Address &sourceAddress);

    /// Set the destination IP address.
    void setDestinationAddress(const Ip4Address &destinationAddress);

    /// The offset of the checksum field in the IPv4 header.
    static constexpr uint32_t CHECKSUM_OFFSET = 10;

private:

    static constexpr uint32_t MIN_HEADER_LENGTH = 20;

    uint8_t version = 4;
    uint8_t headerLength = MIN_HEADER_LENGTH;
    uint16_t payloadLength = 0;
    uint8_t timeToLive = 64;
    Protocol protocol = INVALID;
    Ip4Address sourceAddress;
    Ip4Address destinationAddress;
};

}

#endif
