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

#include "Ip4Module.h"
#include "Ip4Header.h"
#include "kernel/system/System.h"
#include "kernel/service/NetworkService.h"

namespace Network::Ip4 {

Kernel::Logger Ip4Module::log = Kernel::Logger::get("IPv4");

void Ip4Module::readPacket(Util::Stream::ByteArrayInputStream &stream, LayerInformation information, Device::Network::NetworkDevice &device) {
    auto &tmpStream = reinterpret_cast<Util::Stream::ByteArrayInputStream &>(stream);
    auto *buffer = tmpStream.getBuffer() + tmpStream.getPosition();
    uint8_t headerLength = (buffer[0] & 0x0f) * sizeof(uint32_t);
    auto calculatedChecksum = calculateChecksum(buffer, Ip4Header::CHECKSUM_OFFSET, headerLength);
    auto receivedChecksum = (buffer[Ip4Header::CHECKSUM_OFFSET] << 8) | buffer[Ip4Header::CHECKSUM_OFFSET + 1];

    if (receivedChecksum != calculatedChecksum) {
        log.warn("Discarding packet, because of wrong header checksum");
        return;
    }

    auto header = Ip4Header();
    header.read(stream);

    if (header.getVersion() != 4) {
        log.warn("Discarding packet, because of wrong IP version");
        return;
    }

    if (header.getTimeToLive() == 1) {
        log.warn("Discarding packet, because its time to live has expired");
    }

    auto &interface = getInterface(device.getIdentifier());
    if (interface.isTargetOf(header.getDestinationAddress())) {
        if (isNextLayerTypeSupported(header.getProtocol())) {
            invokeNextLayerModule(header.getProtocol(), {header.getSourceAddress(), header.getDestinationAddress(), header.getPayloadLength()}, stream, device);
        } else {
            log.warn("Discarding packet, because of unsupported protocol!");
            return;
        }
    } else {
        log.warn("Discarding packet, because of wrong destination address!");
        return;
    }
}

void Ip4Module::writeHeader(Util::Stream::ByteArrayOutputStream &stream, Device::Network::NetworkDevice &device, const Ip4Address &destinationAddress, Ip4Header::Protocol protocol) {
    auto &networkService = Kernel::System::getService<Kernel::NetworkService>();
    auto &arpModule = networkService.getNetworkStack().getArpModule();
    auto &ip4Module = networkService.getNetworkStack().getIp4Module();

    auto destinationMacAddress = MacAddress();
    if (!arpModule.resolveAddress(destinationAddress, destinationMacAddress, device)) {
        log.warn("Discarding packet, because the destination IPv4 address could not resolved");
        return;
    }

    auto &interface = ip4Module.getInterface(device.getIdentifier());
    Ethernet::EthernetModule::writeHeader(stream, device, destinationMacAddress, Ethernet::EthernetHeader::IP4);

    auto header = Ip4Header();
    header.setSourceAddress(interface.getAddress());
    header.setDestinationAddress(destinationAddress);
    header.setProtocol(protocol);
    header.write(stream);

    auto *buffer = stream.getBuffer() + stream.getPosition() - header.getHeaderLength();
    uint8_t headerLength = (buffer[0] & 0x0f) * sizeof(uint32_t);
    auto checksum = calculateChecksum(buffer, Ip4Header::CHECKSUM_OFFSET, headerLength);
    buffer[Ip4Header::CHECKSUM_OFFSET] = checksum >> 8;
    buffer[Ip4Header::CHECKSUM_OFFSET + 1] = checksum;
}

Ip4Interface& Ip4Module::getInterface(const Util::Memory::String &deviceIdentifier) {
    for (auto *interface : interfaces) {
        if (interface->getDeviceIdentifier() == deviceIdentifier) {
            return *interface;
        }
    }

    Util::Exception::throwException(Util::Exception::INVALID_ARGUMENT, "Ip4Module: Device not found!");
}

void Ip4Module::registerInterface(const Ip4Address &address, const Ip4Address &networkAddress, const Ip4NetworkMask &networkMask, Device::Network::NetworkDevice &device) {
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

}