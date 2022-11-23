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

namespace Network::Ip4 {

Kernel::Logger Ip4Module::log = Kernel::Logger::get("IPv4");

void Ip4Module::readPacket(Util::Stream::InputStream &stream, Device::Network::NetworkDevice &device) {
    auto &tmpStream = reinterpret_cast<Util::Stream::ByteArrayInputStream&>(stream);
    auto *buffer = tmpStream.getBuffer() + tmpStream.getPosition();
    auto calculatedChecksum = Ip4Header::calculateChecksum(buffer);
    auto receivedChecksum = (buffer[10] << 8) | buffer[11];

    if (receivedChecksum != calculatedChecksum) {
        log.warn("Discarding packet because of not matching header checksum");
        return;
    }

    auto header = Ip4Header();
    header.read(stream);

    if (header.getVersion() != 4) {
        log.warn("Discarding packet because of wrong IP version");
        return;
    }

    if (!device.hasAddress(header.getDestinationAddress())) {
        log.warn("Discarding packet because of wrong destination address!");
        return;
    }
}

}