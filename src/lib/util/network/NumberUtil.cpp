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

#include "NumberUtil.h"

#include "lib/util/stream/InputStream.h"
#include "lib/util/stream/OutputStream.h"

namespace Util::Network {

int8_t Util::Network::NumberUtil::read8BitValue(Util::Stream::InputStream &stream) {
    return static_cast<int8_t>(stream.read());
}

uint8_t Util::Network::NumberUtil::readUnsigned8BitValue(Util::Stream::InputStream &stream) {
    return static_cast<uint8_t>(stream.read());
}

int16_t Util::Network::NumberUtil::read16BitValue(Util::Stream::InputStream &stream) {
    return static_cast<int16_t>((stream.read() << 8) | stream.read());
}

uint16_t Util::Network::NumberUtil::readUnsigned16BitValue(Util::Stream::InputStream &stream) {
    return static_cast<uint16_t>(read16BitValue(stream));
}

int32_t Util::Network::NumberUtil::read32BitValue(Util::Stream::InputStream &stream) {
    return static_cast<int32_t>((stream.read() << 24) | (stream.read() << 16) | (stream.read() << 8) | stream.read());
}

uint32_t Util::Network::NumberUtil::readUnsigned32BitValue(Util::Stream::InputStream &stream) {
    return static_cast<uint32_t>(read32BitValue(stream));
}

void Util::Network::NumberUtil::write8BitValue(int8_t value, Util::Stream::OutputStream &stream) {
    stream.write(static_cast<uint8_t>(value));
}

void Util::Network::NumberUtil::writeUnsigned8BitValue(uint8_t value, Util::Stream::OutputStream &stream) {
    stream.write(value);
}

void Util::Network::NumberUtil::write16BitValue(int16_t value, Util::Stream::OutputStream &stream) {
    stream.write(static_cast<uint8_t>(value >> 8));
    stream.write(static_cast<uint8_t>(value));
}

void Util::Network::NumberUtil::writeUnsigned16BitValue(uint16_t value, Util::Stream::OutputStream &stream) {
    write16BitValue(static_cast<int16_t>(value), stream);
}

void Util::Network::NumberUtil::write32BitValue(int32_t value, Util::Stream::OutputStream &stream) {
    stream.write(static_cast<uint8_t>(value >> 24));
    stream.write(static_cast<uint8_t>(value >> 16));
    stream.write(static_cast<uint8_t>(value >> 8));
    stream.write(static_cast<uint8_t>(value));
}

void Util::Network::NumberUtil::writeUnsigned32BitValue(uint32_t value, Util::Stream::OutputStream &stream) {
    write32BitValue(static_cast<int32_t>(value), stream);
}

}