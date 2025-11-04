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

#include "Ip4Header.h"

#include "util/io/stream/InputStream.h"
#include "../../io/stream/NumberUtil.h"
#include "util/network/ip4/Ip4Address.h"

namespace Util {
namespace Io {
class OutputStream;
}  // namespace Stream
}  // namespace Util

namespace Util::Network::Ip4 {

void Ip4Header::read(Io::InputStream &stream) {
    // Read IP version and header length
    const auto versionAndLength = Io::NumberUtil::readUnsigned8BitValue(stream);
    version = versionAndLength >> 4;
    headerLength = (versionAndLength & 0x0f) * sizeof(uint32_t);

    // Discard DSCP and ECN
    stream.read();

    const auto totalLength = Io::NumberUtil::readUnsigned16BitValue(stream);
    payloadLength = totalLength - headerLength;

    // Discard Identification, Flags and Offset
    stream.read();
    stream.read();
    stream.read();
    stream.read();

    timeToLive = Io::NumberUtil::readUnsigned8BitValue(stream);
    protocol = static_cast<Protocol>(Io::NumberUtil::readUnsigned8BitValue(stream));

    // Discard checksum
    stream.read();
    stream.read();

    sourceAddress.read(stream);
    destinationAddress.read(stream);

    // Discard options
    for (uint32_t i = MIN_HEADER_LENGTH; i < headerLength; i++) {
        stream.read();
    }
}

void Ip4Header::write(Io::OutputStream &stream) const {
    // Write IP version and header length
    Io::NumberUtil::writeUnsigned8BitValue(version << 4 | (MIN_HEADER_LENGTH / sizeof(uint32_t)), stream);

    // Write empty DSCP and ECN
    Io::NumberUtil::writeUnsigned8BitValue(0, stream);

    Io::NumberUtil::writeUnsigned16BitValue(headerLength + payloadLength, stream);

    // Write empty Identification, Flags and Offset
    Io::NumberUtil::writeUnsigned32BitValue(0, stream);

    Io::NumberUtil::writeUnsigned8BitValue(timeToLive, stream);
    Io::NumberUtil::writeUnsigned8BitValue(protocol, stream);

    // Write empty checksum
    Io::NumberUtil::writeUnsigned16BitValue(0, stream);

    sourceAddress.write(stream);
    destinationAddress.write(stream);
}

uint8_t Ip4Header::getVersion() const {
    return version;
}

uint16_t Ip4Header::getPayloadLength() const {
    return payloadLength;
}

uint8_t Ip4Header::getTimeToLive() const {
    return timeToLive;
}

Ip4Header::Protocol Ip4Header::getProtocol() const {
    return protocol;
}

const Ip4Address& Ip4Header::getSourceAddress() const {
    return sourceAddress;
}

const Ip4Address& Ip4Header::getDestinationAddress() const {
    return destinationAddress;
}

void Ip4Header::setPayloadLength(const uint16_t payloadLength) {
    Ip4Header::payloadLength = payloadLength;
}

void Ip4Header::setTimeToLive(const uint8_t timeToLive) {
    Ip4Header::timeToLive = timeToLive;
}

void Ip4Header::setProtocol(const Protocol protocol) {
    Ip4Header::protocol = protocol;
}

void Ip4Header::setSourceAddress(const Ip4Address &sourceAddress) {
    Ip4Header::sourceAddress = sourceAddress;
}

void Ip4Header::setDestinationAddress(const Ip4Address &destinationAddress) {
    Ip4Header::destinationAddress = destinationAddress;
}

uint8_t Ip4Header::getHeaderLength() const {
    return headerLength;
}

}