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
 * The network stack is based on a bachelor's thesis, written by Hannes Feil.
 * The original source code can be found here: https://github.com/hhuOS/hhuOS/tree/legacy/network
 */

#include "lib/util/network/ethernet/EthernetHeader.h"

#include "lib/util/network/NumberUtil.h"

namespace Util {
namespace Stream {
class InputStream;
class OutputStream;
}  // namespace Stream
}  // namespace Util

namespace Util::Network::Ethernet {

void EthernetHeader::read(Util::Stream::InputStream &stream) {
    destinationAddress.read(stream);
    sourceAddress.read(stream);
    etherType = static_cast<EtherType>(Util::Network::NumberUtil::readUnsigned16BitValue(stream));
}

void EthernetHeader::write(Util::Stream::OutputStream &stream) {
    destinationAddress.write(stream);
    sourceAddress.write(stream);
    Util::Network::NumberUtil::writeUnsigned16BitValue(etherType, stream);
}

const Util::Network::MacAddress& EthernetHeader::getDestinationAddress() const {
    return destinationAddress;
}

const Util::Network::MacAddress& EthernetHeader::getSourceAddress() const {
    return sourceAddress;
}

EthernetHeader::EtherType EthernetHeader::getEtherType() const {
    return etherType;
}

void EthernetHeader::setDestinationAddress(const Util::Network::MacAddress &address) {
    EthernetHeader::destinationAddress = address;
}

void EthernetHeader::setSourceAddress(const Util::Network::MacAddress &address) {
    EthernetHeader::sourceAddress = address;
}

void EthernetHeader::setEtherType(EthernetHeader::EtherType type) {
    EthernetHeader::etherType = type;
}

}