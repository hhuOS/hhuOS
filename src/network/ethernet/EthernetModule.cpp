/*
 * Copyright (C) 2018-2022 Heinrich-Heine-Universitaet Duesseldorf,
 * Institute of Computer Science, Department Operating Systems
 * Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Hannes Feil, Michael Schoettner
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

#include "EthernetModule.h"
#include "EthernetHeader.h"
#include "network/NumberUtil.h"
#include "EthernetDatagram.h"

namespace Network::Ethernet {

Kernel::Logger EthernetModule::log = Kernel::Logger::get("Ethernet");

bool EthernetModule::checkPacket(const uint8_t *packet, uint32_t length) {
    uint32_t frameCheckSequence = (packet[length - 4] << 24) | (packet[length - 3] << 16) | (packet[length - 2] << 8) | packet[length - 1];
    uint32_t calculatedCheckSequence = calculateCheckSequence(packet, length - 4);
    return frameCheckSequence == calculatedCheckSequence;
}

void EthernetModule::readPacket(Util::Stream::ByteArrayInputStream &stream, LayerInformation information, Device::Network::NetworkDevice &device) {
    auto header = EthernetHeader();
    header.read(stream);

    if (header.getDestinationAddress() != device.getMacAddress()) {
        log.warn("Discarding packet, because of wrong destination address!");
        return;
    }

    auto payloadLength = information.payloadLength - EthernetHeader::HEADER_LENGTH;

    socketLock.acquire();
    for (auto *socket : sockets) {
        auto *datagramBuffer = new uint8_t[payloadLength];
        Util::Memory::Address<uint32_t>(datagramBuffer).copyRange(Util::Memory::Address<uint32_t>(stream.getBuffer() + stream.getPosition()), payloadLength);

        auto *datagram = new EthernetDatagram(datagramBuffer, payloadLength, header.getSourceAddress(), header.getEtherType());
        socket->handleIncomingDatagram(datagram);
    }
    socketLock.release();

    if (!isNextLayerTypeSupported(header.getEtherType())) {
        log.warn("Discarding packet, because of unsupported ether type!");
        return;
    }

    invokeNextLayerModule(header.getEtherType(), {header.getSourceAddress(), header.getDestinationAddress(), payloadLength}, stream, device);
}

uint32_t EthernetModule::calculateCheckSequence(const uint8_t *packet, uint32_t length) {
    // TODO: Calculate check sequence
    return 0;
}

void EthernetModule::writeHeader(Util::Stream::OutputStream &stream, Device::Network::NetworkDevice &device, const MacAddress &destinationAddress, EthernetHeader::EtherType etherType) {
    auto header = EthernetHeader();
    header.setSourceAddress(device.getMacAddress());
    header.setDestinationAddress(destinationAddress);
    header.setEtherType(etherType);
    header.write(stream);
}

void EthernetModule::finalizePacket(Util::Stream::ByteArrayOutputStream &packet) {
    for (uint32_t i = packet.getSize(); i < MINIMUM_PACKET_SIZE - sizeof(uint32_t); i++) {
        NumberUtil::writeUnsigned8BitValue(0, packet);
    }

    auto checkSequence = calculateCheckSequence(packet.getBuffer(), packet.getSize());
    NumberUtil::writeUnsigned32BitValue(checkSequence, packet);
}

bool EthernetModule::registerSocket(EthernetSocket &socket) {
    return sockets.add(&socket);
}

void EthernetModule::deregisterSocket(EthernetSocket &socket) {
    sockets.remove(&socket);
}

}