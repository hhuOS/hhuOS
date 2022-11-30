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

#include "UdpModule.h"
#include "UdpHeader.h"
#include "Ip4PseudoHeader.h"
#include "lib/util/stream/ByteArrayOutputStream.h"

namespace Network::Udp {

Kernel::Logger UdpModule::log = Kernel::Logger::get("UDP");

void UdpModule::readPacket(Util::Stream::ByteArrayInputStream &stream, NetworkModule::LayerInformation information, Device::Network::NetworkDevice &device) {
    auto pseudoHeader = Ip4PseudoHeader(information);
    auto header = UdpHeader();
    header.read(stream);

    auto pseudoHeaderStream = Util::Stream::ByteArrayOutputStream();
    pseudoHeader.write(pseudoHeaderStream);

    auto checksum = calculateChecksum(pseudoHeaderStream.getBuffer(), stream.getBuffer() + stream.getPosition() - UdpHeader::HEADER_SIZE, information.payloadLength);
    if (header.getChecksum() != checksum) {
        log.warn("Discarding packet, because of wrong checksum");
        return;
    }
}

uint16_t UdpModule::calculateChecksum(const uint8_t *pseudoHeader, const uint8_t *datagram, uint16_t datagramLength) {
    uint32_t checksum = 0;
    for (uint16_t i = 0; i < Ip4PseudoHeader::HEADER_SIZE; i += 2) {
        checksum += (pseudoHeader[i] << 8) | pseudoHeader[i + 1];
    }

    for (uint16_t i = 0; i < datagramLength; i += 2) {
        // Ignore checksum field
        if (i == 6) {
            continue;
        }

        if (i == datagramLength - 1) {
            checksum += datagram[i] << 8;
        } else {
            checksum += (datagram[i] << 8) | datagram[i + 1];
        }
    }

    // Add overflow bits
    while (checksum >> 16 > 0) {
        checksum = (checksum >> 16) + (checksum & 0xffff);
    }

    // Complement result
    return ~checksum;
}


}
