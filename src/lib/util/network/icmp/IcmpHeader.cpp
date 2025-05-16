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

#include "lib/util/network/icmp/IcmpHeader.h"

#include "lib/util/network/NumberUtil.h"

namespace Util {
namespace Io {
class InputStream;
class OutputStream;
}  // namespace Stream
}  // namespace Util

namespace Util::Network::Icmp {

void IcmpHeader::read(Util::Io::InputStream &stream) {
    type = static_cast<Type>(Util::Network::NumberUtil::readUnsigned8BitValue(stream));
    code = Util::Network::NumberUtil::readUnsigned8BitValue(stream);
    checksum = Util::Network::NumberUtil::readUnsigned16BitValue(stream);
}

void IcmpHeader::write(Util::Io::OutputStream &stream) {
    Util::Network::NumberUtil::writeUnsigned8BitValue(type, stream);
    Util::Network::NumberUtil::writeUnsigned8BitValue(code, stream);
    Util::Network::NumberUtil::writeUnsigned16BitValue(checksum, stream);
}

IcmpHeader::Type IcmpHeader::getType() const {
    return type;
}

void IcmpHeader::setType(IcmpHeader::Type type) {
    IcmpHeader::type = type;
}

uint8_t IcmpHeader::getCode() const {
    return code;
}

void IcmpHeader::setCode(uint8_t code) {
    IcmpHeader::code = code;
}

uint16_t IcmpHeader::getChecksum() const {
    return checksum;
}

}