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

#include "Ip4Module.h"

#include "lib/util/network/ip4/Ip4Header.h"
#include "kernel/system/System.h"
#include "kernel/service/NetworkService.h"
#include "lib/util/network/ip4/Ip4Datagram.h"
#include "device/network/NetworkDevice.h"
#include "kernel/log/Logger.h"
#include "lib/util/Exception.h"
#include "lib/util/async/Spinlock.h"
#include "lib/util/stream/ByteArrayInputStream.h"
#include "lib/util/stream/ByteArrayOutputStream.h"
#include "lib/util/network/MacAddress.h"
#include "lib/util/network/NetworkAddress.h"
#include "kernel/network/NetworkStack.h"
#include "kernel/network/Socket.h"
#include "kernel/network/arp/ArpModule.h"
#include "lib/util/network/ethernet/EthernetHeader.h"
#include "kernel/network/ethernet/EthernetModule.h"
#include "lib/util/network/ip4/Ip4Address.h"
#include "kernel/network/ip4/Ip4Interface.h"
#include "kernel/network/ip4/Ip4Route.h"
#include "kernel/network/ip4/Ip4RoutingModule.h"
#include "kernel/network/ip4/Ip4Socket.h"

namespace Util {
namespace Network {
namespace Ip4 {
class Ip4NetworkMask;
}  // namespace Ip4
}  // namespace Network
}  // namespace Util

namespace Kernel::Network::Ip4 {

Kernel::Logger Ip4Module::log = Kernel::Logger::get("IPv4");

void Ip4Module::readPacket(Util::Stream::ByteArrayInputStream &stream, LayerInformation information, Device::Network::NetworkDevice &device) {
    auto &tmpStream = reinterpret_cast<Util::Stream::ByteArrayInputStream &>(stream);
    auto *buffer = tmpStream.getBuffer() + tmpStream.getPosition();
    uint8_t headerLength = (buffer[0] & 0x0f) * sizeof(uint32_t);
    auto calculatedChecksum = calculateChecksum(buffer, Util::Network::Ip4::Ip4Header::CHECKSUM_OFFSET, headerLength);
    auto receivedChecksum = (buffer[Util::Network::Ip4::Ip4Header::CHECKSUM_OFFSET] << 8) | buffer[Util::Network::Ip4::Ip4Header::CHECKSUM_OFFSET + 1];

    if (receivedChecksum != calculatedChecksum) {
        log.warn("Discarding packet, because of wrong header checksum");
        return;
    }

    auto header = Util::Network::Ip4::Ip4Header();
    header.read(stream);

    if (header.getVersion() != 4) {
        log.warn("Discarding packet, because of wrong IP version");
        return;
    }

    if (header.getTimeToLive() == 1) {
        log.warn("Discarding packet, because its time to live has expired");
    }

    auto &interface = getInterface(device.getIdentifier());
    if (!interface.isTargetOf(header.getDestinationAddress())) {
        log.warn("Discarding packet, because of wrong destination address!");
        return;
    }

    auto payloadLength = header.getPayloadLength();
    auto *datagramBuffer = stream.getBuffer() + stream.getPosition();

    socketLock.acquire();
    for (auto *socket : socketList) {
        if (socket->getAddress() == Util::Network::Ip4::Ip4Address::ANY || socket->getAddress() == header.getDestinationAddress()) {
            auto *datagram = new Util::Network::Ip4::Ip4Datagram(datagramBuffer, payloadLength, header.getSourceAddress(), header.getProtocol());
            reinterpret_cast<Ip4Socket *>(socket)->handleIncomingDatagram(datagram);
        }
    }
    socketLock.release();

    invokeNextLayerModule(header.getProtocol(), {header.getSourceAddress(), header.getDestinationAddress(), header.getPayloadLength()}, stream, device);
}

const Ip4Interface & Ip4Module::writeHeader(Util::Stream::ByteArrayOutputStream &stream, const Util::Network::Ip4::Ip4Address &sourceAddress,
                                            const Util::Network::Ip4::Ip4Address &destinationAddress, Util::Network::Ip4::Ip4Header::Protocol protocol, uint16_t payloadLength) {
    auto &networkService = Kernel::System::getService<Kernel::NetworkService>();
    auto &arpModule = networkService.getNetworkStack().getArpModule();
    auto &ip4Module = networkService.getNetworkStack().getIp4Module();
    auto route = ip4Module.routingModule.findRoute(sourceAddress, destinationAddress);

    auto destinationMacAddress = Util::Network::MacAddress();
    if (!arpModule.resolveAddress(route.hasNextHop() ? route.getNextHop() : destinationAddress, destinationMacAddress, route.getInterface().getDevice())) {
        Util::Exception::throwException(Util::Exception::INVALID_ARGUMENT, "Discarding packet, because the destination IPv4 address could not resolved");
    }

    Ethernet::EthernetModule::writeHeader(stream, route.getInterface().getDevice(), destinationMacAddress, Util::Network::Ethernet::EthernetHeader::IP4);

    auto header = Util::Network::Ip4::Ip4Header();
    header.setSourceAddress(route.getInterface().getAddress());
    header.setDestinationAddress(destinationAddress);
    header.setProtocol(protocol);
    header.setPayloadLength(payloadLength);
    header.setTimeToLive(64);
    header.write(stream);

    auto *buffer = stream.getBuffer() + stream.getPosition() - header.getHeaderLength();
    uint8_t headerLength = (buffer[0] & 0x0f) * sizeof(uint32_t);
    auto checksum = calculateChecksum(buffer, Util::Network::Ip4::Ip4Header::CHECKSUM_OFFSET, headerLength);
    buffer[Util::Network::Ip4::Ip4Header::CHECKSUM_OFFSET] = checksum >> 8;
    buffer[Util::Network::Ip4::Ip4Header::CHECKSUM_OFFSET + 1] = checksum;

    return route.getInterface();
}

Ip4Interface& Ip4Module::getInterface(const Util::Memory::String &deviceIdentifier) {
    for (auto *interface : interfaces) {
        if (interface->getDeviceIdentifier() == deviceIdentifier) {
            return *interface;
        }
    }

    Util::Exception::throwException(Util::Exception::INVALID_ARGUMENT, "Ip4Module: Device not found!");
}

void Ip4Module::registerInterface(const Util::Network::Ip4::Ip4Address &address, const Util::Network::Ip4::Ip4Address &networkAddress, const Util::Network::Ip4::Ip4NetworkMask &networkMask, Device::Network::NetworkDevice &device) {
    interfaces.add(new Ip4Interface(address, networkAddress, networkMask, device));

    auto &arpModule = Kernel::System::getService<Kernel::NetworkService>().getNetworkStack().getArpModule();
    arpModule.setEntry(address, device.getMacAddress());
}

uint16_t Ip4Module::calculateChecksum(const uint8_t *buffer, uint32_t offset, uint32_t length) {
    uint32_t checksum = 0;
    for (uint8_t i = 0; i < length; i += 2) {
        // Ignore checksum field
        if (i == offset) {
            continue;
        }

        checksum += (buffer[i] << 8) | buffer[i + 1];
    }

    // Add overflow bits
    checksum += reinterpret_cast<uint16_t*>(&checksum)[1];

    // Cut off high bytes
    checksum = static_cast<uint16_t>(checksum);

    // Complement result
    return ~checksum;
}

Ip4RoutingModule& Ip4Module::getRoutingModule() {
    return routingModule;
}

}