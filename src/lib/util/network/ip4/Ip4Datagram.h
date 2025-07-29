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

#ifndef HHUOS_LIB_UTIL_NETWORK_IP4DATAGRAM_H
#define HHUOS_LIB_UTIL_NETWORK_IP4DATAGRAM_H

#include <stdint.h>

#include "network/Datagram.h"
#include "network/ip4/Ip4Header.h"

namespace Util {
namespace Io {
class ByteArrayOutputStream;
}  // namespace Io

namespace Network {
class NetworkAddress;

namespace Ip4 {
class Ip4Address;
}  // namespace Ip4
}  // namespace Network
}  // namespace Util

namespace Util::Network::Ip4 {

/// Specialization of the Datagram class for IPv4 datagrams.
/// This can be used to send and receive IPv4 packets via a `Socket` of type `IP4`.
///
/// ## Example
/// ```c++
/// // Create a new IPv4 socket
/// auto socket = Util::Network::Socket::createSocket(Util::Network::Socket::IP4);
///
/// // Bind the socket to a specific IPv4 address
/// if (!socket.bind(Util::Network::Ip4::Ip4Address("10.0.2.15"))) {
///     printf("Failed to bind socket!\n");
///     return;
/// }
///
/// // Send an IPv4 datagram containing the string "Hello, World!".
/// // As this is no parseable content for any transport layer protocol, we use the protocol `INVALID`.
/// const auto datagram = Util::Network::Ip4::Ip4Datagram(reinterpret_cast<const uint8_t*>("Hello, World!"), 13,
///     destinationAddress, Util::Network::Ip4::Ip4Header::INVALID);
///
/// if (!socket.send(datagram)) {
///     printf("Failed to send datagram!\n");
///     return;
/// }
/// ```
class Ip4Datagram final : public Datagram {
public:
    /// Create a new IPv4 datagram with an uninitialized buffer. The protocol is set to `INVALID`.
    /// This is typically used for receiving datagrams, because in this case the buffer is allocated
    /// by the kernel during the receive system call.
    Ip4Datagram();

    /// Create a new IPv4 datagram with a given buffer and length, remote IPv4 address and protocol.
    /// The buffer's content is copied into the datagram's buffer.
    Ip4Datagram(const uint8_t *buffer, uint16_t length, const Ip4Address &remoteAddress, Ip4Header::Protocol protocol);

    /// Create a new Ethernet datagram from a byte array output stream, remote IPv4 address and protocol.
    /// The stream's content is copied into the datagram's buffer by directly accessing the stream's buffer.
    /// This way, the state of the stream remains unchanged.
    Ip4Datagram(const Io::ByteArrayOutputStream &stream, const NetworkAddress &remoteAddress, Ip4Header::Protocol protocol);

    /// Get the protocol that this datagram is carrying (e.g. ICMP, TCP, UDP).
    [[nodiscard]] Ip4Header::Protocol getProtocol() const;

    /// Set the protocol of this datagram to the one of the given datagram.
    /// This is used by the kernel to copy attributes from a kernel space datagram to a user space datagram.
    void setAttributes(const Datagram &datagram) override;

private:

    Ip4Header::Protocol protocol = Ip4Header::INVALID;
};

}

#endif
