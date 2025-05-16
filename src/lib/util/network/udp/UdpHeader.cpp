/*
 * Copyright (C) 2017-2025 Heinrich Heine University Düsseldorf,
 * Institute of Computer Science, Department Operating Systems
 * Main developers: Christian Gesse <christian.gesse@hhu.de>, Fabian Ruhland <ruhland@hhu.de>
 * Original development team: Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schöttner
 * This project has been supported by several students.
 * A full list of integrated student theses can be found here: https://github.com/hhuOS/hhuOS/wiki/Student-theses
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

#include "lib/util/network/udp/UdpHeader.h"

#include "lib/util/network/NumberUtil.h"

namespace Util {
namespace Io {
class InputStream;
class OutputStream;
}  // namespace Stream
}  // namespace Util

namespace Util::Network::Udp {

void UdpHeader::read(Util::Io::InputStream &stream) {
    sourcePort = Util::Network::NumberUtil::readUnsigned16BitValue(stream);
    destinationPort = Util::Network::NumberUtil::readUnsigned16BitValue(stream);
    datagramLength = Util::Network::NumberUtil::readUnsigned16BitValue(stream);
    checksum = Util::Network::NumberUtil::readUnsigned16BitValue(stream);
}

void UdpHeader::write(Util::Io::OutputStream &stream) const {
    Util::Network::NumberUtil::writeUnsigned16BitValue(sourcePort, stream);
    Util::Network::NumberUtil::writeUnsigned16BitValue(destinationPort, stream);
    Util::Network::NumberUtil::writeUnsigned16BitValue(datagramLength, stream);
    Util::Network::NumberUtil::writeUnsigned16BitValue(checksum, stream);
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

uint16_t UdpHeader::getDatagramLength() const {
    return datagramLength;
}

void UdpHeader::setDatagramLength(uint16_t length) {
    UdpHeader::datagramLength = length;
}

uint16_t UdpHeader::getChecksum() const {
    return checksum;
}

}