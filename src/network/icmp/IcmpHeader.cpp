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

#include "IcmpHeader.h"
#include "network/NumberUtil.h"

namespace Network::Icmp {

void IcmpHeader::read(Util::Stream::InputStream &stream) {
    type = static_cast<Type>(NumberUtil::readUnsigned8BitValue(stream));
    code = NumberUtil::readUnsigned8BitValue(stream);
    checksum = NumberUtil::readUnsigned16BitValue(stream);
}

void IcmpHeader::write(Util::Stream::OutputStream &stream) {
    NumberUtil::writeUnsigned8BitValue(type, stream);
    NumberUtil::writeUnsigned8BitValue(code, stream);
    NumberUtil::writeUnsigned16BitValue(checksum, stream);
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