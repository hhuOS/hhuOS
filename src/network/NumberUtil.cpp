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

#include "NumberUtil.h"

#include "lib/util/stream/InputStream.h"
#include "lib/util/stream/OutputStream.h"

namespace Network {

int8_t NumberUtil::read8BitValue(Util::Stream::InputStream &stream) {
    return static_cast<int8_t>(stream.read());
}

uint8_t NumberUtil::readUnsigned8BitValue(Util::Stream::InputStream &stream) {
    return static_cast<uint8_t>(stream.read());
}

int16_t NumberUtil::read16BitValue(Util::Stream::InputStream &stream) {
    return static_cast<int16_t>((stream.read() << 8) | stream.read());
}

uint16_t NumberUtil::readUnsigned16BitValue(Util::Stream::InputStream &stream) {
    return static_cast<uint16_t>(read16BitValue(stream));
}

int32_t NumberUtil::read32BitValue(Util::Stream::InputStream &stream) {
    return static_cast<int32_t>((stream.read() << 24) | (stream.read() << 16) | (stream.read() << 8) | stream.read());
}

uint32_t NumberUtil::readUnsigned32BitValue(Util::Stream::InputStream &stream) {
    return static_cast<uint32_t>(read32BitValue(stream));
}

void NumberUtil::write8BitValue(int8_t value, Util::Stream::OutputStream &stream) {
    stream.write(static_cast<uint8_t>(value));
}

void NumberUtil::writeUnsigned8BitValue(uint8_t value, Util::Stream::OutputStream &stream) {
    stream.write(value);
}

void NumberUtil::write16BitValue(int16_t value, Util::Stream::OutputStream &stream) {
    stream.write(static_cast<uint8_t>(value >> 8));
    stream.write(static_cast<uint8_t>(value));
}

void NumberUtil::writeUnsigned16BitValue(uint16_t value, Util::Stream::OutputStream &stream) {
    write16BitValue(static_cast<int16_t>(value), stream);
}

void NumberUtil::write32BitValue(int32_t value, Util::Stream::OutputStream &stream) {
    stream.write(static_cast<uint8_t>(value >> 24));
    stream.write(static_cast<uint8_t>(value >> 16));
    stream.write(static_cast<uint8_t>(value >> 8));
    stream.write(static_cast<uint8_t>(value));
}

void NumberUtil::writeUnsigned32BitValue(uint32_t value, Util::Stream::OutputStream &stream) {
    write32BitValue(static_cast<int32_t>(value), stream);
}

}