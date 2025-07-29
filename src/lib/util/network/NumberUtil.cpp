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

#include "NumberUtil.h"

#include "io/stream/InputStream.h"
#include "io/stream/OutputStream.h"

namespace Util::Network::NumberUtil {

int8_t read8BitValue(Io::InputStream &stream) {
    return static_cast<int8_t>(stream.read());
}

uint8_t readUnsigned8BitValue(Io::InputStream &stream) {
    return static_cast<uint8_t>(stream.read());
}

int16_t read16BitValue(Io::InputStream &stream) {
    return static_cast<int16_t>((stream.read() << 8) | stream.read());
}

uint16_t readUnsigned16BitValue(Io::InputStream &stream) {
    return static_cast<uint16_t>(read16BitValue(stream));
}

int32_t read32BitValue(Io::InputStream &stream) {
    return stream.read() << 24 | stream.read() << 16 | stream.read() << 8 | stream.read();
}

uint32_t readUnsigned32BitValue(Io::InputStream &stream) {
    return static_cast<uint32_t>(read32BitValue(stream));
}

void write8BitValue(const int8_t value, Io::OutputStream &stream) {
    stream.write(static_cast<uint8_t>(value));
}

void writeUnsigned8BitValue(const uint8_t value, Io::OutputStream &stream) {
    stream.write(value);
}

void write16BitValue(const int16_t value, Io::OutputStream &stream) {
    stream.write(static_cast<uint8_t>(value >> 8));
    stream.write(static_cast<uint8_t>(value));
}

void writeUnsigned16BitValue(const uint16_t value, Io::OutputStream &stream) {
    write16BitValue(static_cast<int16_t>(value), stream);
}

void write32BitValue(const int32_t value, Io::OutputStream &stream) {
    stream.write(static_cast<uint8_t>(value >> 24));
    stream.write(static_cast<uint8_t>(value >> 16));
    stream.write(static_cast<uint8_t>(value >> 8));
    stream.write(static_cast<uint8_t>(value));
}

void writeUnsigned32BitValue(const uint32_t value, Io::OutputStream &stream) {
    write32BitValue(static_cast<int32_t>(value), stream);
}

}