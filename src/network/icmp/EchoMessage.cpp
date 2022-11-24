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

#include "EchoMessage.h"
#include "network/NumberUtil.h"
#include "lib/util/memory/Address.h"

namespace Network::Icmp {

EchoMessage::~EchoMessage() {
    delete data;
}

uint16_t EchoMessage::getIdentifier() const {
    return identifier;
}

void EchoMessage::setIdentifier(uint16_t identifier) {
    EchoMessage::identifier = identifier;
}

uint16_t EchoMessage::getSequenceNumber() const {
    return sequenceNumber;
}

void EchoMessage::setSequenceNumber(uint16_t sequenceNumber) {
    EchoMessage::sequenceNumber = sequenceNumber;
}

void EchoMessage::read(Util::Stream::InputStream &stream, uint32_t length) {
    identifier = NumberUtil::readUnsigned16BitValue(stream);
    sequenceNumber = NumberUtil::readUnsigned16BitValue(stream);

    dataLength = length - 2 * sizeof (uint16_t);
    if (length == dataLength) {
        return;
    }

    data = new uint8_t[dataLength];
    stream.read(data, 0, dataLength);
}

void EchoMessage::write(Util::Stream::OutputStream &stream) const {
    NumberUtil::writeUnsigned16BitValue(identifier, stream);
    NumberUtil::writeUnsigned16BitValue(sequenceNumber, stream);
    stream.write(data, 0, dataLength);
}

uint8_t* EchoMessage::getData() const {
    return data;
}

void EchoMessage::setData(uint8_t *data, uint32_t length) {
    delete EchoMessage::data;
    EchoMessage::data = new uint8_t[length];
    dataLength = length;

    auto source = Util::Memory::Address<uint32_t>(data);
    auto target = Util::Memory::Address<uint32_t>(EchoMessage::data);
    target.copyRange(source, length);
}

uint32_t EchoMessage::getDataLength() const {
    return dataLength;
}

}