/*
 * Copyright (C) 2018-2022 Heinrich-Heine-Universitaet Duesseldorf,
 * Institute of Computer Science, Department Operating Systems
 * Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Hannes Feil,  Michael Schoettner
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

#include "network/NumberUtil.h"
#include "ArpFrame.h"

namespace Network::Arp {

Kernel::Logger ArpFrame::log = Kernel::Logger::get("Arp");

void ArpFrame::read(Util::Stream::InputStream &stream) {
    auto hardwareAddressType = static_cast<HardwareAddressType>(NumberUtil::readUnsigned16BitValue(stream));
    auto protocolAddressType = static_cast<ProtocolAddressType>(NumberUtil::readUnsigned16BitValue(stream));
    auto hardwareAddressSize = static_cast<uint8_t>(stream.read());
    auto protocolAddressSize = static_cast<uint8_t>(stream.read());
    operation = static_cast<Operation>(NumberUtil::readUnsigned16BitValue(stream));

    if (hardwareAddressType != ETHERNET) {
        log.warn("Received arp frame with unsupported hardware address type %04x", hardwareAddressType);
        discardPacket(stream, hardwareAddressSize, protocolAddressSize);
        return;
    }

    if (protocolAddressType != IP4) {
        log.warn("Received arp frame with unsupported hardware address type %04x", hardwareAddressType);
        discardPacket(stream, hardwareAddressSize, protocolAddressSize);
        return;
    }

    sourceMacAddress.readAddress(stream);
    sourceIpAddress.readAddress(stream);
    targetMacAddress.readAddress(stream);
    targetIpAddress.readAddress(stream);
}

MacAddress ArpFrame::getSourceMacAddress() const {
    return sourceMacAddress;
}

MacAddress ArpFrame::getTargetMacAddress() const {
    return targetMacAddress;
}

Ip4::Ip4Address ArpFrame::getSourceIpAddress() const {
    return sourceIpAddress;
}

Ip4::Ip4Address ArpFrame::getTargetIpAddress() const {
    return targetIpAddress;
}

void
ArpFrame::discardPacket(Util::Stream::InputStream &stream, uint8_t hardwareAddressSize, uint8_t protocolAddressSize) {
    auto *hardwareAddressBuffer = new uint8_t[hardwareAddressSize];
    auto *protocolAddressBuffer = new uint8_t[protocolAddressSize];

    stream.read(hardwareAddressBuffer, 0, hardwareAddressSize);
    stream.read(protocolAddressBuffer, 0, protocolAddressSize);
    stream.read(hardwareAddressBuffer, 0, hardwareAddressSize);
    stream.read(protocolAddressBuffer, 0, protocolAddressSize);
}

}