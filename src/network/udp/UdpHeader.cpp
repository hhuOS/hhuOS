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

#include "UdpHeader.h"
#include "network/NumberUtil.h"

namespace Network::Udp {

void UdpHeader::read(Util::Stream::InputStream &stream) {
    sourcePort = NumberUtil::readUnsigned16BitValue(stream);
    destinationPort = NumberUtil::readUnsigned16BitValue(stream);
    length = NumberUtil::readUnsigned16BitValue(stream);
    checksum = NumberUtil::readUnsigned16BitValue(stream);
}

void UdpHeader::write(Util::Stream::OutputStream &stream) const {
    NumberUtil::writeUnsigned16BitValue(sourcePort, stream);
    NumberUtil::writeUnsigned16BitValue(destinationPort, stream);
    NumberUtil::writeUnsigned16BitValue(length, stream);
    NumberUtil::writeUnsigned16BitValue(checksum, stream);
}

uint16_t UdpHeader::getSourcePort() const {
    return sourcePort;
}

void UdpHeader::setSourcePort(uint16_t sourcePort) {
    UdpHeader::sourcePort = sourcePort;
}

uint16_t UdpHeader::getDestinationPort() const {
    return destinationPort;
}

void UdpHeader::setDestinationPort(uint16_t targetPort) {
    UdpHeader::destinationPort = targetPort;
}

uint16_t UdpHeader::getLength() const {
    return length;
}

void UdpHeader::setLength(uint16_t length) {
    UdpHeader::length = length;
}

uint16_t UdpHeader::getChecksum() const {
    return checksum;
}

void UdpHeader::setChecksum(uint16_t checksum) {
    UdpHeader::checksum = checksum;
}

}