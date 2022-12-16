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

#include "Ip4PseudoHeader.h"

#include "network/NumberUtil.h"
#include "network/ip4/Ip4Header.h"

namespace Util {
namespace Stream {
class OutputStream;
}  // namespace Stream
}  // namespace Util

namespace Network::Udp {

Ip4PseudoHeader::Ip4PseudoHeader(const Ip4::Ip4Address &sourceAddress, const Ip4::Ip4Address &destinationAddress, uint16_t datagramLength) :
        sourceAddress(sourceAddress),
        destinationAddress(destinationAddress),
        datagramLength(datagramLength) {}

Ip4PseudoHeader::Ip4PseudoHeader(const NetworkModule::LayerInformation &information) :
        sourceAddress(reinterpret_cast<const Ip4::Ip4Address&>(information.sourceAddress)),
        destinationAddress(reinterpret_cast<const Ip4::Ip4Address&>(information.destinationAddress)),
        datagramLength(information.payloadLength) {}

void Ip4PseudoHeader::write(Util::Stream::OutputStream &stream) const {
    sourceAddress.write(stream);
    destinationAddress.write(stream);
    NumberUtil::writeUnsigned16BitValue(Ip4::Ip4Header::UDP, stream);
    NumberUtil::writeUnsigned16BitValue(datagramLength, stream);
}

const Ip4::Ip4Address& Ip4PseudoHeader::getSourceAddress() const {
    return sourceAddress;
}

const Ip4::Ip4Address& Ip4PseudoHeader::getDestinationAddress() const {
    return destinationAddress;
}

uint16_t Ip4PseudoHeader::getDatagramLength() const {
    return datagramLength;
}

}