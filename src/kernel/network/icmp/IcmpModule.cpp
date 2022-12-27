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
 * The UDP/IP stack is based on a bachelor's thesis, written by Hannes Feil.
 * The original source code can be found here: https://github.com/hhuOS/hhuOS/tree/legacy/network
 */

#include "IcmpModule.h"

#include "lib/util/network/icmp/IcmpHeader.h"
#include "lib/util/network/icmp/EchoHeader.h"
#include "lib/util/network/icmp/IcmpDatagram.h"
#include "IcmpSocket.h"
#include "kernel/network/icmp/IcmpModule.h"
#include "device/network/NetworkDevice.h"
#include "kernel/log/Logger.h"
#include "lib/util/async/Spinlock.h"
#include "lib/util/data/ArrayList.h"
#include "lib/util/data/Iterator.h"
#include "lib/util/stream/ByteArrayInputStream.h"
#include "lib/util/stream/ByteArrayOutputStream.h"
#include "lib/util/network/NetworkAddress.h"
#include "kernel/network/Socket.h"
#include "kernel/network/ethernet/EthernetModule.h"
#include "lib/util/network/ip4/Ip4Header.h"
#include "kernel/network/ip4/Ip4Interface.h"
#include "kernel/network/ip4/Ip4Module.h"
#include "lib/util/network/ip4/Ip4Address.h"

namespace Kernel::Network::Icmp {

Kernel::Logger IcmpModule::log = Kernel::Logger::get("ICMP");

void IcmpModule::readPacket(Util::Stream::ByteArrayInputStream &stream, LayerInformation information, Device::Network::NetworkDevice &device) {
    auto *buffer = stream.getBuffer() + stream.getPosition();
    auto calculatedChecksum = Ip4::Ip4Module::calculateChecksum(buffer, Util::Network::Icmp::IcmpHeader::CHECKSUM_OFFSET, stream.getRemaining());
    auto receivedChecksum = (buffer[Util::Network::Icmp::IcmpHeader::CHECKSUM_OFFSET] << 8) | buffer[Util::Network::Icmp::IcmpHeader::CHECKSUM_OFFSET + 1];

    if (receivedChecksum != calculatedChecksum) {
        log.warn("Discarding packet, because of wrong header checksum");
        return;
    }

    auto &sourceAddress = reinterpret_cast<const Util::Network::Ip4::Ip4Address &>(information.sourceAddress);
    auto &destinationAddress = reinterpret_cast<const Util::Network::Ip4::Ip4Address &>(information.destinationAddress);

    auto header = Util::Network::Icmp::IcmpHeader();
    header.read(stream);

    switch (header.getType()) {
        case Util::Network::Icmp::IcmpHeader::ECHO_REQUEST: {
            auto requestHeader = Util::Network::Icmp::EchoHeader();
            requestHeader.read(stream);
            sendEchoReply(destinationAddress, sourceAddress, requestHeader, stream.getBuffer() + stream.getPosition(), stream.getRemaining(), device);
            break;
        }
        default: {
            auto payloadLength = information.payloadLength - Util::Network::Icmp::IcmpHeader::HEADER_LENGTH;
            auto *datagramBuffer = stream.getBuffer() + stream.getPosition();

            socketLock.acquire();
            for (auto *socket: socketList) {
                if (socket->getAddress() == Util::Network::Ip4::Ip4Address::ANY || socket->getAddress() == information.destinationAddress) {
                    auto *datagram = new Util::Network::Icmp::IcmpDatagram(datagramBuffer, payloadLength, sourceAddress, header.getType(), header.getCode());
                    reinterpret_cast<IcmpSocket *>(socket)->handleIncomingDatagram(datagram);
                }
            }
            socketLock.release();
        }
    }
}

void IcmpModule::writePacket(Util::Network::Icmp::IcmpHeader::Type type, uint8_t code, const Util::Network::Ip4::Ip4Address &sourceAddress,
                             const Util::Network::Ip4::Ip4Address &destinationAddress, const uint8_t *buffer, uint16_t length) {
    auto packet = Util::Stream::ByteArrayOutputStream();
    auto datagramLength = length + Util::Network::Icmp::IcmpHeader::HEADER_LENGTH;

    // Write IPv4 and Ethernet headers
    auto &sourceInterface = Ip4::Ip4Module::writeHeader(packet, sourceAddress, destinationAddress, Util::Network::Ip4::Ip4Header::ICMP, datagramLength);

    // Write ICMP header
    auto header = Util::Network::Icmp::IcmpHeader();
    header.setType(type);
    header.setCode(code);
    header.write(packet);
    auto positionAfterHeaders = packet.getPosition();

    // Write packet
    packet.write(buffer, 0, length);

    // Calculate and write checksum
    auto *datagramBuffer = packet.getBuffer() + packet.getPosition() - datagramLength;
    auto checksum = Ip4::Ip4Module::calculateChecksum(datagramBuffer, Util::Network::Icmp::IcmpHeader::CHECKSUM_OFFSET, datagramLength);

    auto *checksumPointer = packet.getBuffer() + (positionAfterHeaders - sizeof(uint16_t));
    checksumPointer[0] = checksum >> 8;
    checksumPointer[1] = checksum;

    // Finalize and send packet
    Ethernet::EthernetModule::finalizePacket(packet);
    sourceInterface.getDevice().sendPacket(packet.getBuffer(), packet.getLength());
}

void
IcmpModule::sendEchoReply(const Util::Network::Ip4::Ip4Address &sourceAddress, const Util::Network::Ip4::Ip4Address &destinationAddress,
                          const Util::Network::Icmp::EchoHeader &requestHeader, const uint8_t *buffer, uint16_t length, Device::Network::NetworkDevice &device) {
    auto packet = Util::Stream::ByteArrayOutputStream();
    auto replyHeader = Util::Network::Icmp::EchoHeader();
    replyHeader.setIdentifier(requestHeader.getIdentifier());
    replyHeader.setSequenceNumber(requestHeader.getSequenceNumber());

    replyHeader.write(packet);
    packet.write(buffer, 0, length);

    writePacket(Util::Network::Icmp::IcmpHeader::ECHO_REPLY, 0, sourceAddress, destinationAddress, packet.getBuffer(), packet.getLength());
}

}