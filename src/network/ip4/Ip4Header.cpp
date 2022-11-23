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

#include "Ip4Header.h"
#include "network/NumberUtil.h"

namespace Network::Ip4 {

void Ip4Header::read(Util::Stream::InputStream &stream) {
    auto versionAndLength = NumberUtil::readUnsigned8BitValue(stream);
    version = versionAndLength >> 4;
    uint8_t headerLength = (versionAndLength & 0x0f) * sizeof(uint32_t);

    // Discard DSCP and ECN
    stream.read();

    auto totalLength = NumberUtil::readUnsigned16BitValue(stream);
    payloadLength = totalLength - headerLength;

    // Discard Identification, Flags and Offset
    stream.read();
    stream.read();
    stream.read();
    stream.read();

    timeToLive = NumberUtil::readUnsigned8BitValue(stream);
    protocol = static_cast<Protocol>(NumberUtil::readUnsigned8BitValue(stream));

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

uint16_t Ip4Header::calculateChecksum(const uint8_t *buffer) {
    uint8_t headerLength = (buffer[0] & 0x0f) * sizeof(uint32_t);
    uint32_t checksum = 0;

    for (uint8_t i = 0; i < headerLength; i += 2) {
        // Ignore checksum field
        if (i == 10) {
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

Ip4Address Ip4Header::getSourceAddress() const {
    return sourceAddress;
}

Ip4Address Ip4Header::getDestinationAddress() const {
    return destinationAddress;
}

}