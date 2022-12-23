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

#include "lib/util/network/NumberUtil.h"
#include "ArpHeader.h"

namespace Util {
namespace Stream {
class InputStream;
class OutputStream;
}  // namespace Stream
}  // namespace Util

namespace Kernel::Network::Arp {

void ArpHeader::read(Util::Stream::InputStream &stream) {
    hardwareAddressType = static_cast<HardwareAddressType>(Util::Network::NumberUtil::readUnsigned16BitValue(stream));
    protocolAddressType = static_cast<ProtocolAddressType>(Util::Network::NumberUtil::readUnsigned16BitValue(stream));
    hardwareAddressSize = Util::Network::NumberUtil::readUnsigned8BitValue(stream);
    protocolAddressSize = Util::Network::NumberUtil::readUnsigned8BitValue(stream);
    operation = static_cast<Operation>(Util::Network::NumberUtil::readUnsigned16BitValue(stream));
}

void ArpHeader::write(Util::Stream::OutputStream &stream) {
    Util::Network::NumberUtil::writeUnsigned16BitValue(hardwareAddressType, stream);
    Util::Network::NumberUtil::writeUnsigned16BitValue(protocolAddressType, stream);
    Util::Network::NumberUtil::writeUnsigned8BitValue(hardwareAddressSize, stream);
    Util::Network::NumberUtil::writeUnsigned8BitValue(protocolAddressSize, stream);
    Util::Network::NumberUtil::writeUnsigned16BitValue(operation, stream);
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