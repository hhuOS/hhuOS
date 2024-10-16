/*
 * Copyright (C) 2018-2024 Heinrich-Heine-Universitaet Duesseldorf,
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
#include "kernel/service/NetworkService.h"
#include "lib/util/network/ip4/Ip4Datagram.h"
#include "device/network/NetworkDevice.h"
#include "kernel/log/Log.h"
#include "lib/util/base/Exception.h"
#include "lib/util/async/Spinlock.h"
#include "lib/util/io/stream/ByteArrayInputStream.h"
#include "lib/util/io/stream/ByteArrayOutputStream.h"
#include "lib/util/network/MacAddress.h"
#include "lib/util/network/NetworkAddress.h"
#include "kernel/network/NetworkStack.h"
#include "kernel/network/Socket.h"
#include "kernel/network/arp/ArpModule.h"
#include "lib/util/network/ethernet/EthernetHeader.h"
#include "kernel/network/ethernet/EthernetModule.h"
#include "lib/util/network/ip4/Ip4Address.h"
#include "kernel/network/ip4/Ip4Interface.h"
#include "kernel/network/ip4/Ip4RoutingModule.h"
#include "kernel/network/ip4/Ip4Socket.h"
#include "lib/util/network/ip4/Ip4Route.h"
#include "lib/util/network/ip4/Ip4SubnetAddress.h"
#include "lib/util/collection/Iterator.h"
#include "kernel/service/Service.h"

namespace Kernel::Network::Ip4 {

void Ip4Module::readPacket(Util::Io::ByteArrayInputStream &stream, [[maybe_unused]] LayerInformation information, Device::Network::NetworkDevice &device) {
    auto &tmpStream = reinterpret_cast<Util::Io::ByteArrayInputStream&>(stream);
    auto *buffer = tmpStream.getBuffer() + tmpStream.getPosition();
    uint8_t headerLength = (buffer[0] & 0x0f) * sizeof(uint32_t);
    auto calculatedChecksum = calculateChecksum(buffer, Util::Network::Ip4::Ip4Header::CHECKSUM_OFFSET, headerLength);
    auto receivedChecksum = (buffer[Util::Network::Ip4::Ip4Header::CHECKSUM_OFFSET] << 8) | buffer[Util::Network::Ip4::Ip4Header::CHECKSUM_OFFSET + 1];

    if (receivedChecksum != calculatedChecksum) {
        LOG_WARN("Discarding packet, because of wrong header checksum");
        return;
    }

    auto header = Util::Network::Ip4::Ip4Header();
    header.read(stream);

    if (header.getVersion() != 4) {
        LOG_WARN("Discarding packet, because of wrong IP version");
        return;
    }

    if (header.getTimeToLive() == 1) {
        LOG_WARN("Discarding packet, because its time to live has expired");
    }

    if (getTargetInterfaces(header.getDestinationAddress()).length() == 0) {
        LOG_WARN("Discarding packet, because of wrong destination address!");
        return;
    }

    auto payloadLength = header.getPayloadLength();
    auto *datagramBuffer = stream.getBuffer() + stream.getPosition();

    socketLock.acquire();
    for (auto *socket : socketList) {
        if (socket->getAddress() == Util::Network::Ip4::Ip4Address::ANY || socket->getAddress() == header.getDestinationAddress()) {
            auto *datagram = new Util::Network::Ip4::Ip4Datagram(datagramBuffer, payloadLength, header.getSourceAddress(), header.getProtocol());
            reinterpret_cast<Ip4Socket*>(socket)->handleIncomingDatagram(datagram);
        }
    }
    socketLock.release();

    invokeNextLayerModule(header.getProtocol(), {header.getSourceAddress(), header.getDestinationAddress(), header.getPayloadLength()}, stream, device);
}

Ip4Interface Ip4Module::writeHeader(Util::Io::ByteArrayOutputStream &stream, const Util::Network::Ip4::Ip4Address &sourceAddress, const Util::Network::Ip4::Ip4Address &destinationAddress, Util::Network::Ip4::Ip4Header::Protocol protocol, uint16_t payloadLength) {
    auto &networkService = Kernel::Service::getService<Kernel::NetworkService>();
    auto &arpModule = networkService.getNetworkStack().getArpModule();
    auto &ip4Module = networkService.getNetworkStack().getIp4Module();
    auto route = ip4Module.routingModule.findRoute(sourceAddress, destinationAddress);
    auto interface = ip4Module.getTargetInterfaces(route.getSourceAddress())[0];

    auto destinationMacAddress = Util::Network::MacAddress();
    if (!arpModule.resolveAddress(route.hasNextHop() ? route.getNextHop() : destinationAddress, destinationMacAddress, interface)) {
        Util::Exception::throwException(Util::Exception::INVALID_ARGUMENT, "Discarding packet, because the destination IPv4 address could not be resolved");
    }

    Ethernet::EthernetModule::writeHeader(stream, interface.getDevice(), destinationMacAddress, Util::Network::Ethernet::EthernetHeader::IP4);

    auto header = Util::Network::Ip4::Ip4Header();
    header.setSourceAddress(route.getSourceAddress());
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

    return interface;
}

Util::Array<Ip4Interface> Ip4Module::getInterfaces(const Util::String &deviceIdentifier) {
    auto ret = Util::ArrayList<Ip4Interface>();

    lock.acquire();
    for (auto interface : interfaces) {
        if (interface.getDeviceIdentifier() == deviceIdentifier) {
            ret.add(interface);
        }
    }
    lock.release();

    return ret.toArray();
}

Util::Array<Ip4Interface> Ip4Module::getTargetInterfaces(const Util::Network::Ip4::Ip4Address &address) {
    auto ret = Util::ArrayList<Ip4Interface>();

    lock.acquire();
    for (const auto &interface : interfaces) {
        if (interface.isTargetOf(address)) {
            ret.add(interface);
        }
    }
    lock.release();

    return ret.toArray();
}

bool Ip4Module::registerInterface(const Util::Network::Ip4::Ip4SubnetAddress &address, Device::Network::NetworkDevice &device) {
    lock.acquire();
    auto interface = Ip4Interface(address, device);
    if (interfaces.contains(interface) || interface.getIp4Address() == Util::Network::Ip4::Ip4Address::ANY) {
        lock.release();
        return false;
    }

    auto ret = interfaces.add(interface);
    lock.release();

    if (ret) {
        auto &arpModule = Kernel::Service::getService<Kernel::NetworkService>().getNetworkStack().getArpModule();
        arpModule.setEntry(address.getIp4Address(), device.getMacAddress());
    }

    return ret;
}

bool Ip4Module::removeInterface(const Util::Network::Ip4::Ip4SubnetAddress &address, const Util::String &deviceIdentifier) {
    lock.acquire();
    for (const auto &interface : interfaces) {
        if (interface.getSubnetAddress() == address && interface.getDeviceIdentifier() == deviceIdentifier) {
            auto &arpModule = Kernel::Service::getService<Kernel::NetworkService>().getNetworkStack().getArpModule();
            arpModule.removeEntry(interface.getIp4Address());

            routingModule.removeRoute(address, deviceIdentifier);
            interfaces.remove(interface);

            lock.release();
            return true;
        }
    }

    lock.release();
    return false;
}

Ip4RoutingModule &Ip4Module::getRoutingModule() {
    return routingModule;
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

}