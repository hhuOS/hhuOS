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

#include "UdpModule.h"

#include "lib/util/network/udp/UdpHeader.h"
#include "Ip4PseudoHeader.h"
#include "lib/util/stream/ByteArrayOutputStream.h"
#include "network/ip4/Ip4Module.h"
#include "network/ethernet/EthernetModule.h"
#include "device/network/NetworkDevice.h"
#include "kernel/log/Logger.h"
#include "lib/util/async/Spinlock.h"
#include "lib/util/data/ArrayList.h"
#include "lib/util/data/Iterator.h"
#include "lib/util/stream/ByteArrayInputStream.h"
#include "lib/util/network/NetworkAddress.h"
#include "network/Socket.h"
#include "lib/util/network/ip4/Ip4Header.h"
#include "network/ip4/Ip4Interface.h"
#include "lib/util/network/ip4/Ip4PortAddress.h"
#include "lib/util/network/udp/UdpDatagram.h"
#include "network/udp/UdpSocket.h"

namespace Util {
namespace Network {
namespace Ip4 {
class Ip4Address;
}  // namespace Ip4
}  // namespace Network
}  // namespace Util

namespace Network::Udp {

Kernel::Logger UdpModule::log = Kernel::Logger::get("UDP");

bool UdpModule::registerSocket(Socket &socket) {
    socketLock.acquire();
    for (const auto *currentSocket : socketList) {
        if (currentSocket->getAddress() == socket.getAddress()) {
            return socketLock.releaseAndReturn(false);
        }
    }

    socketList.add(&socket);
    return socketLock.releaseAndReturn(true);
}

void UdpModule::readPacket(Util::Stream::ByteArrayInputStream &stream, NetworkModule::LayerInformation information, Device::Network::NetworkDevice &device) {
    auto pseudoHeader = Ip4PseudoHeader(information);
    auto header = Util::Network::Udp::UdpHeader();
    header.read(stream);

    auto pseudoHeaderStream = Util::Stream::ByteArrayOutputStream();
    pseudoHeader.write(pseudoHeaderStream);

    auto checksum = calculateChecksum(pseudoHeaderStream.getBuffer(), stream.getData() + stream.getPosition() - Util::Network::Udp::UdpHeader::HEADER_SIZE, information.payloadLength);
    if (header.getChecksum() != checksum) {
        log.warn("Discarding packet, because of wrong checksum");
        return;
    }

    auto destinationAddress = Util::Network::Ip4::Ip4PortAddress(pseudoHeader.getDestinationAddress(), header.getDestinationPort());
    auto payloadLength = header.getDatagramLength() - Util::Network::Udp::UdpHeader::HEADER_SIZE;
    auto *datagramBuffer = stream.getData() + stream.getPosition();

    socketLock.acquire();
    for (auto *socket : socketList) {
        if (socket->getAddress() != destinationAddress) {
            continue;
        }

        auto *datagram = new Util::Network::Udp::UdpDatagram(datagramBuffer, payloadLength, Util::Network::Ip4::Ip4PortAddress(reinterpret_cast<const Util::Network::Ip4::Ip4Address&>(information.sourceAddress), header.getSourcePort()));
        reinterpret_cast<UdpSocket*>(socket)->handleIncomingDatagram(datagram);
    }
    socketLock.release();
}

void UdpModule::writePacket(uint16_t sourcePort, uint16_t destinationPort, const Util::Network::Ip4::Ip4Address &destinationAddress, const uint8_t *buffer, uint16_t length) {
    auto packet = Util::Stream::ByteArrayOutputStream();
    auto datagramLength = length + Util::Network::Udp::UdpHeader::HEADER_SIZE;

    // Write IPv4 and Ethernet headers
    auto &sourceInterface = Ip4::Ip4Module::writeHeader(packet, destinationAddress, Util::Network::Ip4::Ip4Header::UDP, datagramLength);

    // Write UDP header
    auto udpHeader = Util::Network::Udp::UdpHeader();
    udpHeader.setSourcePort(sourcePort);
    udpHeader.setDestinationPort(destinationPort);
    udpHeader.setDatagramLength(datagramLength);
    udpHeader.write(packet);
    auto positionAfterHeaders = packet.getPosition();

    // Write packet
    packet.write(buffer, 0, length);

    // Calculate and write checksum
    auto pseudoHeader = Ip4PseudoHeader(sourceInterface.getAddress(), destinationAddress, datagramLength);
    auto pseudoHeaderStream = Util::Stream::ByteArrayOutputStream();
    pseudoHeader.write(pseudoHeaderStream);

    auto checksum = calculateChecksum(pseudoHeaderStream.getBuffer(), packet.getBuffer() + (positionAfterHeaders - Util::Network::Udp::UdpHeader::HEADER_SIZE), datagramLength);
    auto *checksumPointer = packet.getBuffer() + (positionAfterHeaders - sizeof(uint16_t));
    checksumPointer[0] = checksum >> 8;
    checksumPointer[1] = checksum;

    // Finalize and send packet
    Ethernet::EthernetModule::finalizePacket(packet);
    sourceInterface.getDevice().sendPacket(packet.getBuffer(), packet.getLength());
}

uint16_t UdpModule::calculateChecksum(const uint8_t *pseudoHeader, const uint8_t *datagram, uint16_t datagramLength) {
    uint32_t checksum = 0;
    for (uint16_t i = 0; i < Ip4PseudoHeader::HEADER_SIZE; i += 2) {
        checksum += (pseudoHeader[i] << 8) | pseudoHeader[i + 1];
    }

    for (uint16_t i = 0; i < datagramLength; i += 2) {
        // Ignore checksum field
        if (i == 6) {
            continue;
        }

        if (i == datagramLength - 1) {
            checksum += datagram[i] << 8;
        } else {
            checksum += (datagram[i] << 8) | datagram[i + 1];
        }
    }

    // Add overflow bits
    while (checksum >> 16 > 0) {
        checksum = (checksum >> 16) + (checksum & 0xffff);
    }

    // Complement result
    return ~checksum;
}

}
