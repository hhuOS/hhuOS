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

#include "IcmpModule.h"
#include "IcmpHeader.h"
#include "EchoMessage.h"
#include "lib/util/stream/ByteArrayOutputStream.h"
#include "kernel/service/NetworkService.h"

namespace Network::Icmp {

Kernel::Logger IcmpModule::log = Kernel::Logger::get("ICMP");

void IcmpModule::readPacket(Util::Stream::ByteArrayInputStream &stream, LayerInformation information, Device::Network::NetworkDevice &device) {
    auto *buffer = stream.getBuffer() + stream.getPosition();
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
            auto request = EchoMessage();
            request.read(stream, information.payloadLength);
            sendEchoReply(reinterpret_cast<const Ip4::Ip4Address&>(information.sourceAddress), request, device);
            break;
        }
        case IcmpHeader::ECHO_REPLY:
            log.info("Received echo reply from [%s]", static_cast<const char*>(information.sourceAddress.toString()));
            break;
        default:
            log.warn("Discarding ICMP packet, because of unsupported type");
    }
}

void IcmpModule::sendEchoReply(const Ip4::Ip4Address &destinationAddress, const EchoMessage &request, Device::Network::NetworkDevice &device) {
    auto header = IcmpHeader();
    header.setType(IcmpHeader::ECHO_REPLY);

    auto reply = EchoMessage();
    reply.setIdentifier(request.getIdentifier());
    reply.setSequenceNumber(request.getSequenceNumber() + 1);
    reply.setData(request.getData(), request.getDataLength());

    auto packet = Util::Stream::ByteArrayOutputStream();
    Network::Ip4::Ip4Module::writeHeader(packet, device, reinterpret_cast<const Ip4::Ip4Address&>(destinationAddress), Ip4::Ip4Header::ICMP);
    header.write(packet);
    reply.write(packet);

    auto *buffer = packet.getBuffer() + packet.getPosition() - IcmpHeader::HEADER_LENGTH - reply.getDataLength();
    auto checksum = Ip4::Ip4Module::calculateChecksum(buffer, IcmpHeader::CHECKSUM_OFFSET, IcmpHeader::HEADER_LENGTH + reply.getDataLength());
    buffer[IcmpHeader::CHECKSUM_OFFSET] = checksum >> 8;
    buffer[IcmpHeader::CHECKSUM_OFFSET + 1] = checksum;

    Ethernet::EthernetModule::finalizePacket(packet);
    device.sendPacket(packet.getBuffer(), packet.getSize());
}

}