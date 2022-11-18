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
#include "ArpHeader.h"

namespace Network::Arp {

void ArpHeader::read(Util::Stream::InputStream &stream) {
    hardwareAddressType = static_cast<HardwareAddressType>(NumberUtil::readUnsigned16BitValue(stream));
    protocolAddressType = static_cast<ProtocolAddressType>(NumberUtil::readUnsigned16BitValue(stream));
    hardwareAddressSize = NumberUtil::readUnsigned8BitValue(stream);
    protocolAddressSize = NumberUtil::readUnsigned8BitValue(stream);
    operation = static_cast<Operation>(NumberUtil::readUnsigned16BitValue(stream));
}

void ArpHeader::write(Util::Stream::OutputStream &stream) {
    NumberUtil::writeUnsigned16BitValue(hardwareAddressType, stream);
    NumberUtil::writeUnsigned16BitValue(protocolAddressType, stream);
    NumberUtil::writeUnsigned8BitValue(hardwareAddressSize, stream);
    NumberUtil::writeUnsigned8BitValue(protocolAddressSize, stream);
    NumberUtil::writeUnsigned16BitValue(operation, stream);
}

ArpHeader::HardwareAddressType ArpHeader::getHardwareAddressType() const {
    return hardwareAddressType;
}

uint8_t ArpHeader::getHardwareAddressSize() const {
    return hardwareAddressSize;
}

ArpHeader::ProtocolAddressType ArpHeader::getProtocolAddressType() const {
    return protocolAddressType;
}

uint8_t ArpHeader::getProtocolAddressSize() const {
    return protocolAddressSize;
}

ArpHeader::Operation ArpHeader::getOperation() const {
    return operation;
}

void ArpHeader::setOperation(ArpHeader::Operation operation) {
    ArpHeader::operation = operation;
}

}