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

#include "EthernetFrame.h"
#include "network/NumberUtil.h"
#include "network/arp/ArpFrame.h"

namespace Network::Ethernet {

Kernel::Logger EthernetFrame::log = Kernel::Logger::get("Ethernet");

void EthernetFrame::read(Util::Stream::InputStream &stream) {
    destinationAddress.readAddress(stream);
    sourceAddress.readAddress(stream);
    etherType = static_cast<EtherType>(NumberUtil::readUnsigned16BitValue(stream));

    switch (etherType) {
        case IP4:
            break;
        case ARP: {
            auto arpFrame = Arp::ArpFrame();
            arpFrame.read(stream);
            break;
        }
        case IP6:
            break;
        default:
            log.warn("Received ethernet frame with unsupported ether type %04x", etherType);
            // TODO: Discard packet
            return;
    }

    checkSequence = NumberUtil::readUnsigned32BitValue(stream);
}

MacAddress EthernetFrame::getDestinationAddress() const {
    return destinationAddress;
}

MacAddress EthernetFrame::getSourceAddress() const {
    return sourceAddress;
}

EthernetFrame::EtherType EthernetFrame::getEtherType() const {
    return etherType;
}

uint32_t EthernetFrame::getCheckSequence() const {
    return checkSequence;
}

}