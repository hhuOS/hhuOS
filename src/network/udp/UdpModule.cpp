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
 */

#include "UdpModule.h"
#include "UdpHeader.h"
#include "Ip4PseudoHeader.h"
#include "lib/util/stream/ByteArrayOutputStream.h"
#include "network/ip4/Ip4Module.h"
#include "network/ethernet/EthernetModule.h"

namespace Network::Udp {

Kernel::Logger UdpModule::log = Kernel::Logger::get("UDP");

void UdpModule::readPacket(Util::Stream::ByteArrayInputStream &stream, NetworkModule::LayerInformation information, Device::Network::NetworkDevice &device) {
    auto pseudoHeader = Ip4PseudoHeader(information);
    auto header = UdpHeader();
    header.read(stream);

    auto pseudoHeaderStream = Util::Stream::ByteArrayOutputStream();
    pseudoHeader.write(pseudoHeaderStream);

    auto checksum = calculateChecksum(pseudoHeaderStream.getBuffer(), stream.getBuffer() + stream.getPosition() - UdpHeader::HEADER_SIZE, information.payloadLength);
    if (header.getChecksum() != checksum) {
        log.warn("Discarding packet, because of wrong checksum");
        return;
    }

    auto dataLength = header.getDatagramLength() - UdpHeader::HEADER_SIZE;
    auto *buffer = new uint8_t[dataLength];
    Util::Memory::Address<uint32_t>(buffer).copyRange(Util::Memory::Address<uint32_t>(stream.getBuffer() + stream.getPosition()), dataLength);

    socketLock.acquire();
    if (!socketMap.containsKey(header.getDestinationPort())) {
        log.warn("Discarding packet, because of wrong destination port");
        return;
    }

    auto *socket = socketMap.get(header.getDestinationPort());
    socketLock.release();

    auto *datagram = new UdpDatagram(buffer, dataLength, Ip4::Ip4PortAddress(reinterpret_cast<const Ip4::Ip4Address&>(information.sourceAddress), header.getSourcePort()));
    socket->handleIncomingDatagram(datagram);
}

void UdpModule::writePacket(uint16_t sourcePort, uint16_t destinationPort, const Ip4::Ip4Address &destinationAddress, const uint8_t *buffer, uint16_t length) {
    auto packet = Util::Stream::ByteArrayOutputStream();
    auto datagramLength = length +  UdpHeader::HEADER_SIZE;

    // Write IPv4 and Ethernet headers
    auto &sourceInterface = Ip4::Ip4Module::writeHeader(packet, destinationAddress, Ip4::Ip4Header::UDP, datagramLength);

    // Write UDP header
    auto udpHeader = UdpHeader();
    udpHeader.setSourcePort(sourcePort);
    udpHeader.setDestinationPort(destinationPort);
    udpHeader.setDatagramLength(datagramLength);
    udpHeader.write(packet);
    auto positionAfterHeaders = packet.getPosition();

    // Write packet
    packet.write(buffer, 0, datagramLength);

    // Calculate and write checksum
    auto pseudoHeader = Ip4PseudoHeader(sourceInterface.getAddress(), destinationAddress, datagramLength);
    auto pseudoHeaderStream = Util::Stream::ByteArrayOutputStream();
    pseudoHeader.write(pseudoHeaderStream);

    auto checksum = calculateChecksum(pseudoHeaderStream.getBuffer(), packet.getBuffer() + (positionAfterHeaders - UdpHeader::HEADER_SIZE), datagramLength);
    auto *checksumPointer = packet.getBuffer() + (positionAfterHeaders - sizeof(uint16_t));
    checksumPointer[0] = checksum >> 8;
    checksumPointer[1] = checksum;

    // Finalize and send packet
    Ethernet::EthernetModule::finalizePacket(packet);
    sourceInterface.getDevice().sendPacket(packet.getBuffer(), packet.getSize());
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

bool UdpModule::registerSocket(UdpSocket &socket) {
    socketLock.acquire();
    if (socketMap.containsKey(socket.getPort())) {
        return socketLock.releaseAndReturn(false);
    }

    socketMap.put(socket.getPort(), &socket);
    return socketLock.releaseAndReturn(true);
}

void UdpModule::deregisterSocket(UdpSocket &socket) {
    socketLock.acquire();
    if (socketMap.containsKey(socket.getPort())) {
        socketMap.remove(socket.getPort());
    }

    socketMap.remove(socket.getPort());
    socketLock.release();
}


}
