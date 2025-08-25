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

#ifndef HHUOS_LIB_UTIL_NETWORK_ICMPDATAGRAM_H
#define HHUOS_LIB_UTIL_NETWORK_ICMPDATAGRAM_H

#include <stdint.h>

#include "network/Datagram.h"
#include "network/icmp/IcmpHeader.h"

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

namespace Util::Network::Icmp {

/// Specialization of the `Datagram` class for ICMP datagrams.
/// This can be used to send and receive ICMP messages via a `Socket` of type `ICMP`.
///
/// ## Example
/// ```c++
/// // Create a new ICMP socket
/// auto socket = Util::Network::Socket::createSocket(Util::Network::Socket::ICMP);
///
/// // Bind the socket to the "0.0.0.0" address to listen for all ICMP messages.
/// if (!socket.bind(Util::Network::Ip4::Ip4Address::ANY)) {
///     printf("Failed to bind socket!\n");
///     return;
/// }
///
/// // Create data for an ICMP Echo Request.
/// auto packet = Util::Io::ByteArrayOutputStream(); // This will be the ICMP packet.
///
/// // Create an Echo Header with identifier and sequence number.
/// auto echoHeader = Util::Network::Icmp::EchoHeader();
/// echoHeader.setIdentifier(0);
/// echoHeader.setSequenceNumber(0);
///
/// // Create a timestamp (the payload of the ICMP Echo Request).
/// auto timestamp = Util::Time::Timestamp::getSystemTime().toMilliseconds();
///
/// // Writ the Echo Header and timestamp to the packet.
/// echoHeader.write(packet);
/// Util::Io::NumberUtil::writeUnsigned32BitValue(timestamp, packet);
///
/// // Create an ICMP datagram with the packet data, destination address, type, and code.
/// auto request = Util::Network::Icmp::IcmpDatagram(packet, Util::Network::Ip4::Ip4Address("10.0.2.2"),
///     Util::Network::Icmp::IcmpHeader::ECHO_REQUEST, 0);
///
/// // Send the ICMP Echo Request via the socket.
/// if (!socket.send(request)) {
///     printf("Failed to send ICMP Echo Request!\n");
///     return;
/// }
///
/// // Wait for an ICMP Echo Reply.
/// auto reply = Util::Network::Icmp::IcmpDatagram();
///
/// while (true) {
///     if (!socket.receive(reply)) {
///         printf("Failed to receive ICMP Echo Reply!\n");
///         return;
///     }
///
///     // Since we can receive all types of ICMP messages, we need to filter for the `ECHO_REPLY` type.
///     if (reply.getType() != Util::Network::Icmp::IcmpHeader::ECHO_REPLY) {
///         continue;
///     }
///
///     // Wrap the received datagram in a ByteArrayInputStream to easily read data from it.
///     auto receivedPacket = Util::Io::ByteArrayInputStream(reply.getData(), reply.getLength());
///
///     // Read the echo header from the received packet.
///     echoHeader.read(receivedPacket);
///
///     // Check if the identifier and sequence number match the ones we sent.
///     // If they do not match, the reply was probably meant for another request, and we should ignore it.
///     if (echoHeader.getIdentifier() != 0 || echoHeader.getSequenceNumber() != 0) {
///         continue;
///     }
///
///     // Read the timestamp from the received packet (this should be the same as the one we sent).
///     const auto sourceTimestamp = Util::Io::NumberUtil::readUnsigned32BitValue(receivedPacket);
///
///     // Calculate the round-trip time by subtracting the source timestamp from the current system time.
///     const auto currentTimestamp = Util::Time::Timestamp::getSystemTime().toMilliseconds();
///     const uint32_t roundTripTime = currentTimestamp - sourceTimestamp;
///
///     printf("Received ICMP Echo Reply (RTT: %u ms)\n", roundTripTime);
///     break; // Exit the loop after receiving a valid reply.
/// }
/// ```
class IcmpDatagram final : public Datagram {

public:
    /// Create a new ICMP datagram with an uninitialized buffer.
    /// This is typically used for receiving datagrams, because in this case the buffer is allocated
    /// by the kernel during the receive system call.
    IcmpDatagram();

    /// Create a new ICMP datagram with a given buffer and length, and a remote IPv4 address.
    /// The buffer's content is copied into the datagram's buffer.
    IcmpDatagram(const uint8_t *buffer, uint16_t length, const Ip4::Ip4Address &remoteAddress,
        IcmpHeader::Type type, uint8_t code);

    /// Create a new ICMP datagram from a byte array output stream and a remote IPv4 address.
    /// The stream's content is copied into the datagram's buffer by directly accessing the stream's buffer.
    /// This way, the state of the stream remains unchanged.
    IcmpDatagram(const Io::ByteArrayOutputStream &stream, const Ip4::Ip4Address &remoteAddress,
        IcmpHeader::Type type, uint8_t code);

    /// Get the ICMP type.
    [[nodiscard]] IcmpHeader::Type getType() const;

    /// Get the ICMP code.
    [[nodiscard]] uint8_t getCode() const;

    /// Set the ICMP type and code of this datagram to the one of the given datagram.
    /// This is used by the kernel to copy attributes from a kernel space datagram to a user space datagram.
    void setAttributes(const Datagram &datagram) override;

private:

    IcmpHeader::Type type = IcmpHeader::ECHO_REQUEST;
    uint8_t code = 0;
};

}

#endif
