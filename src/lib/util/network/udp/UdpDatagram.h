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

#ifndef HHUOS_LIB_UTIL_NETWORK_UDPDATAGRAM_H
#define HHUOS_LIB_UTIL_NETWORK_UDPDATAGRAM_H

#include <stdint.h>

#include "network/Datagram.h"

namespace Util {
namespace Io {
class ByteArrayOutputStream;
}  // namespace Io

namespace Network {
class NetworkAddress;

namespace Ip4 {
class Ip4PortAddress;
}  // namespace Ip4
}  // namespace Network
}  // namespace Util

namespace Util::Network::Udp {

/// Specialization of the Datagram class for UDP datagrams.
/// This can be used to send and receive UDP datagrams via a `Socket` of type `UDP`.
///
/// ## Example
/// ```c++
/// // Create a new UDP socket
/// auto socket = Util::Network::Socket::createSocket(Util::Network::Socket::UDP);
///
/// // Bind the socket to a specific address
/// if (!socket.bind(Util::Network::Ip4::Ip4PortAddress("10.0.2.15:1797"))) {
///     Util::System::out << "Failed to bind socket!"
///         << Util::Io::PrintStream::ln << Util::Io::PrintStream::flush;
///     return;
/// }
///
/// // Create a datagram to send (Payload: "Hello, World!", Destination: "10.0.2.2:1856")
/// const auto destinationAddress = Util::Network::Ip4::Ip4PortAddress("10.0.2.2:1856");
/// const auto datagram = Util::Network::Udp::UdpDatagram(
///     reinterpret_cast<const uint8_t*>("Hello, World!"), 13, destinationAddress);
///
/// // Send the datagram via the socket
/// if (!socket.send(datagram)) {
///     Util::System::out << "Failed to send datagram!"
///         << Util::Io::PrintStream::ln << Util::Io::PrintStream::flush;
///     return;
/// }
///
/// // Wait for a response (e.g., an Echo Reply)
/// auto receivedDatagram = Util::Network::Udp::UdpDatagram();
/// if (!socket.receive(receivedDatagram)) {
///     Util::System::out << "Failed to receive datagram!"
///         << Util::Io::PrintStream::ln << Util::Io::PrintStream::flush;
///     return;
/// }
///
/// // Interpret the received datagram as string and print it
/// const auto message = Util::String(receivedDatagram.getData(), receivedDatagram.getLength())
/// Util::System::out << "Received: " << message << Util::Io::PrintStream::ln << Util::Io::PrintStream::flush;
/// ```
class UdpDatagram final : public Datagram {
public:
    /// Create a new UDP datagram with an uninitialized buffer.
    /// This is typically used for receiving datagrams, because in this case the buffer is allocated
    /// by the kernel during the receive system call.
    UdpDatagram();

    /// Create a new UDP datagram with a given buffer and length, and a remote address.
    /// The buffer's content is copied into the datagram's buffer.
    UdpDatagram(const uint8_t *buffer, uint16_t length, const Ip4::Ip4PortAddress &remoteAddress);

    /// Create a new UDP datagram from a byte array output stream and a remote address.
    /// The stream's content is copied into the datagram's buffer by directly accessing the stream's buffer.
    /// This way, the state of the stream remains unchanged.
    UdpDatagram(const Io::ByteArrayOutputStream &stream, const NetworkAddress &remoteAddress);

    /// Get the remote port of the application that sent or will receive this datagram.
    [[nodiscard]] uint16_t getRemotePort() const;

    /// Set attributes of this datagram based on another datagram.
    /// Since this class has no specific attributes, this method does nothing.
    /// It is provided to fulfill the interface contract of the Datagram class.
    void setAttributes(const Datagram &datagram) override;
};

}

#endif
