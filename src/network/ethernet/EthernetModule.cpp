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

#include "EthernetModule.h"
#include "EthernetHeader.h"

namespace Network::Ethernet {

Kernel::Logger EthernetModule::log = Kernel::Logger::get("Ethernet");

bool EthernetModule::checkPacket(const uint8_t *packet, uint32_t length) {
    // TODO: Check frame check sequence
    //       uint32_t frameCheckSequence = (packet[length - 4] << 24) | (packet[length - 3] << 16) | (packet[length - 2] << 8) | packet[length - 1];
    auto etherType = (packet[12] << 8) | packet[13];
    return isNextLayerTypeSupported(etherType);
}

void EthernetModule::readPacket(Util::Stream::InputStream &stream) {
    auto ethernetHeader = EthernetHeader();
    ethernetHeader.read(stream);
    auto etherType = ethernetHeader.getEtherType();

    if (!isNextLayerTypeSupported(etherType)) {
        // Should never happen, since unsupported packets are discarded by checkPacket()
        Util::Exception::throwException(Util::Exception::ILLEGAL_STATE, "Trying to parse ethernet frame with unsupported ether type!");
    }

    invokeNextLayerModule(etherType, stream);
}

}