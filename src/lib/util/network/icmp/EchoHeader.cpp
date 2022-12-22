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

#include "lib/util/network/icmp/EchoHeader.h"

#include "lib/util/network/NumberUtil.h"

namespace Util {
namespace Stream {
class InputStream;
class OutputStream;
}  // namespace Stream
}  // namespace Util

namespace Util::Network::Icmp {

uint16_t EchoHeader::getIdentifier() const {
    return identifier;
}

void EchoHeader::setIdentifier(uint16_t identifier) {
    EchoHeader::identifier = identifier;
}

uint16_t EchoHeader::getSequenceNumber() const {
    return sequenceNumber;
}

void EchoHeader::setSequenceNumber(uint16_t sequenceNumber) {
    EchoHeader::sequenceNumber = sequenceNumber;
}

void EchoHeader::read(Util::Stream::InputStream &stream) {
    identifier = Util::Network::NumberUtil::readUnsigned16BitValue(stream);
    sequenceNumber = Util::Network::NumberUtil::readUnsigned16BitValue(stream);
}

void EchoHeader::write(Util::Stream::OutputStream &stream) const {
    Util::Network::NumberUtil::writeUnsigned16BitValue(identifier, stream);
    Util::Network::NumberUtil::writeUnsigned16BitValue(sequenceNumber, stream);
}

}