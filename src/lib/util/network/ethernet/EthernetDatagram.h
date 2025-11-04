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

#ifndef HHUOS_LIB_UTIL_NETWORK_ETHERNETDATAGRAM_H
#define HHUOS_LIB_UTIL_NETWORK_ETHERNETDATAGRAM_H

#include <stdint.h>

#include "util/network/Datagram.h"
#include "util/network/ethernet/EthernetHeader.h"

namespace Util {
namespace Io {
class ByteArrayOutputStream;
}  // namespace Io

namespace Network {
class MacAddress;
class NetworkAddress;
}  // namespace Network
}  // namespace Util

namespace Util::Network::Ethernet {

/// Specialization of the `Datagram` class for Ethernet frames.
/// This can be used to send and receive Ethernet frames via a `Socket` of type `ETHERNET`.
///
/// ## Example
/// ```c++
/// // Create a new Ethernet socket
/// auto socket = Util::Network::Socket::createSocket(Util::Network::Socket::ETHERNET);
///
/// // Read MAC address of device "eth0" from file system
/// auto macFile = Util::Io::FileInputStream("/device/eth0/mac");
/// auto macAddress = Util::Network::MacAddress(macFile.readLine().content);
///
/// // Bind the socket to the read MAC address
/// if (!socket.bind(macAddress)) {
///     Util::System::out << "Failed to bind socket!" << Util::Io::PrintStream::ln << Util::Io::PrintStream::flush;
///     return;
/// }
///
/// // Send an Ethernet datagram containing the string "Hello, World!".
/// // As this is no parseable content for any network protocol, we use the EtherType `INVALID`.
/// const auto datagram = Util::Network::Ethernet::EthernetDatagram( reinterpret_cast<const uint8_t*>("Hello, World!"),
///     13, destinationAddress, Util::Network::Ethernet::EthernetHeader::INVALID);
///
/// if (!socket.send(datagram)) {
///     Util::System::out << "Failed to send datagram!" << Util::Io::PrintStream::ln << Util::Io::PrintStream::flush;
///     return;
/// }
/// ```
class EthernetDatagram final : public Datagram {
public:
    /// Create a new Ethernet datagram with an uninitialized buffer.
    /// This is typically used for receiving datagrams, because in this case the buffer is allocated
    /// by the kernel during the receive system call.
    EthernetDatagram();

    /// Create a new Ethernet datagram with a given buffer and length, and a remote MAC address.
    /// The buffer's content is copied into the datagram's buffer.
    EthernetDatagram(const uint8_t *buffer, uint16_t length,
                     const MacAddress &remoteAddress, EthernetHeader::EtherType type);

    /// Create a new Ethernet datagram from a byte array output stream and a remote MAC address.
    /// The stream's content is copied into the datagram's buffer by directly accessing the stream's buffer.
    /// This way, the state of the stream remains unchanged.
    EthernetDatagram(const Io::ByteArrayOutputStream &stream,
        const NetworkAddress &remoteAddress, EthernetHeader::EtherType type);

    /// Get the EtherType of the Ethernet datagram.
    /// The EtherType indicates the protocol encapsulated in the Ethernet frame (e.g. IPv4, ARP, etc.).
    [[nodiscard]] EthernetHeader::EtherType getEtherType() const;

    /// Set the EtherType of this Ethernet datagram to the one of the given datagram.
    /// This is used by the kernel to copy attributes from a kernel space datagram to a user space datagram.
    void setAttributes(const Datagram &datagram) override;

private:

    EthernetHeader::EtherType type = EthernetHeader::INVALID;
};

}

#endif
