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

#include "EthernetHeader.h"
#include "network/NumberUtil.h"
#include "network/arp/ArpHeader.h"

namespace Network::Ethernet {

void EthernetHeader::read(Util::Stream::InputStream &stream) {
    destinationAddress.read(stream);
    sourceAddress.read(stream);
    etherType = static_cast<EtherType>(NumberUtil::readUnsigned16BitValue(stream));
}

void EthernetHeader::write(Util::Stream::OutputStream &stream) {
    destinationAddress.write(stream);
    sourceAddress.write(stream);
    NumberUtil::writeUnsigned16BitValue(etherType, stream);
}

const MacAddress& EthernetHeader::getDestinationAddress() const {
    return destinationAddress;
}

const MacAddress& EthernetHeader::getSourceAddress() const {
    return sourceAddress;
}

EthernetHeader::EtherType EthernetHeader::getEtherType() const {
    return etherType;
}

void EthernetHeader::setDestinationAddress(const MacAddress &address) {
    EthernetHeader::destinationAddress = address;
}

void EthernetHeader::setSourceAddress(const MacAddress &address) {
    EthernetHeader::sourceAddress = address;
}

void EthernetHeader::setEtherType(EthernetHeader::EtherType type) {
    EthernetHeader::etherType = type;
}

}