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

#include "kernel/service/NetworkService.h"
#include "IcmpModule.h"
#include "IcmpHeader.h"
#include "EchoHeader.h"
#include "IcmpDatagram.h"
#include "IcmpSocket.h"

namespace Network::Icmp {

Kernel::Logger IcmpModule::log = Kernel::Logger::get("ICMP");

void IcmpModule::readPacket(Util::Stream::ByteArrayInputStream &stream, LayerInformation information, Device::Network::NetworkDevice &device) {
    auto *buffer = stream.getData() + stream.getPosition();
    auto calculatedChecksum = Ip4::Ip4Module::calculateChecksum(buffer, IcmpHeader::CHECKSUM_OFFSET, stream.getRemaining());
    auto receivedChecksum = (buffer[IcmpHeader::CHECKSUM_OFFSET] << 8) | buffer[IcmpHeader::CHECKSUM_OFFSET + 1];

    if (receivedChecksum != calculatedChecksum) {
        log.warn("Discarding packet, because of wrong header checksum");
        return;
    }

    auto header = IcmpHeader();
    header.read(stream);

    switch (header.getType()) {
        case IcmpHeader::ECHO_REQUEST: {
            auto requestHeader = EchoHeader();
            requestHeader.read(stream);
            sendEchoReply(reinterpret_cast<const Ip4::Ip4Address &>(information.sourceAddress), requestHeader, stream.getData() + stream.getPosition(), stream.getRemaining(), device);
            break;
        }
        default: {
            auto payloadLength = information.payloadLength - IcmpHeader::HEADER_LENGTH;
            auto *datagramBuffer = stream.getData() + stream.getPosition();

            socketLock.acquire();
            for (auto *socket: socketList) {
                if (socket->getAddress() != information.destinationAddress) {
                    continue;
                }

                auto *datagram = new IcmpDatagram(datagramBuffer, payloadLength, reinterpret_cast<const Ip4::Ip4Address&>(information.sourceAddress), header.getType(), header.getCode());
                reinterpret_cast<IcmpSocket*>(socket)->handleIncomingDatagram(datagram);
            }
            socketLock.release();
        }
    }
}

void IcmpModule::writePacket(IcmpHeader::Type type, uint8_t code, const Ip4::Ip4Address &destinationAddress, const uint8_t *buffer, uint16_t length) {
    auto packet = Util::Stream::ByteArrayOutputStream();
    auto datagramLength = length + IcmpHeader::HEADER_LENGTH;

    // Write IPv4 and Ethernet headers
    auto &sourceInterface = Ip4::Ip4Module::writeHeader(packet, destinationAddress, Ip4::Ip4Header::ICMP, datagramLength);

    // Write ICMP header
    auto header = IcmpHeader();
    header.setType(type);
    header.setCode(code);
    header.write(packet);
    auto positionAfterHeaders = packet.getPosition();

    // Write packet
    packet.write(buffer, 0, length);

    // Calculate and write checksum
    auto *datagramBuffer = packet.getBuffer() + packet.getPosition() - datagramLength;
    auto checksum = Ip4::Ip4Module::calculateChecksum(datagramBuffer, IcmpHeader::CHECKSUM_OFFSET, datagramLength);

    auto *checksumPointer = packet.getBuffer() + (positionAfterHeaders - sizeof(uint16_t));
    checksumPointer[0] = checksum >> 8;
    checksumPointer[1] = checksum;

    // Finalize and send packet
    Ethernet::EthernetModule::finalizePacket(packet);
    sourceInterface.getDevice().sendPacket(packet.getBuffer(), packet.getLength());
}

void IcmpModule::sendEchoReply(const Ip4::Ip4Address &destinationAddress, const EchoHeader &requestHeader, const uint8_t *buffer, uint16_t length, Device::Network::NetworkDevice &device) {
    auto packet = Util::Stream::ByteArrayOutputStream();
    auto replyHeader = EchoHeader();
    replyHeader.setIdentifier(requestHeader.getIdentifier());
    replyHeader.setSequenceNumber(requestHeader.getSequenceNumber());

    replyHeader.write(packet);
    packet.write(buffer, 0, length);

    writePacket(IcmpHeader::ECHO_REPLY, 0, destinationAddress, packet.getBuffer(), packet.getLength());
}

}