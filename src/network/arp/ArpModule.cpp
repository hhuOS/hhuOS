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

#include "ArpModule.h"

namespace Network::Arp {

Kernel::Logger ArpModule::log = Kernel::Logger::get("Arp");

void ArpModule::readPacket(Util::Stream::InputStream &stream) {
    auto arpHeader = ArpHeader();
    arpHeader.read(stream);

    if (arpHeader.getHardwareAddressType() != ArpHeader::ETHERNET) {
        log.warn("Received arp frame with unsupported hardware address type %04x", arpHeader.getHardwareAddressType());
        discardPacket(stream, arpHeader);
        return;
    }

    if (arpHeader.getProtocolAddressType() != ArpHeader::IP4) {
        log.warn("Received arp frame with unsupported protocol address type %04x", arpHeader.getProtocolAddressType());
        discardPacket(stream, arpHeader);
        return;
    }

    auto sourceMacAddress = MacAddress();
    auto targetMacAddress = MacAddress();
    auto sourceIpAddress = Ip4::Ip4Address();
    auto targetIpAddress = Ip4::Ip4Address();

    sourceMacAddress.readAddress(stream);
    sourceIpAddress.readAddress(stream);
    targetMacAddress.readAddress(stream);
    targetIpAddress.readAddress(stream);

    switch (arpHeader.getOperation()) {
        case ArpHeader::REQUEST:
            // TODO: Handle arp request
            break;
        case ArpHeader::REPLY:
            // TODO: Handle arp answer
            break;
        default:
            log.warn("Received arp frame with unsupported operation type %04x", arpHeader.getOperation());
    }
}

void ArpModule::discardPacket(Util::Stream::InputStream &stream, const ArpHeader &arpHeader) {
    auto *hardwareAddressBuffer = new uint8_t[arpHeader.getHardwareAddressSize()];
    auto *protocolAddressBuffer = new uint8_t[arpHeader.getProtocolAddressSize()];

    stream.read(hardwareAddressBuffer, 0, arpHeader.getHardwareAddressSize());
    stream.read(protocolAddressBuffer, 0, arpHeader.getProtocolAddressSize());
    stream.read(hardwareAddressBuffer, 0, arpHeader.getHardwareAddressSize());
    stream.read(protocolAddressBuffer, 0, arpHeader.getProtocolAddressSize());
}

}