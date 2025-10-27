/*
 * Copyright (C) 2017-2026 Heinrich Heine University Düsseldorf,
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

#ifndef HHUOS_LIB_UTIL_STREAM_NUMBERUTIL_H
#define HHUOS_LIB_UTIL_STREAM_NUMBERUTIL_H

#include <stdint.h>

#include "util/io/stream/InputStream.h"
#include "util/io/stream/OutputStream.h"

namespace Util {
namespace Io {

/// Provides utility functions for reading numbers from input streams and writing numbers to output streams.
/// The streams are expected to provide raw bytes. For parsing numbers from text streams, use a `ScanStream`.
/// These functions are used a lot in the network stack, to parse network headers and write network packets.
namespace NumberUtil {

/// Read a signed 8-bit value from the given input stream.
inline int8_t read8BitValue(InputStream &stream) {
    return static_cast<int8_t>(stream.read());
}

/// Read an unsigned 8-bit value from the given input stream.
inline uint8_t readUnsigned8BitValue(InputStream &stream) {
    return static_cast<uint8_t>(stream.read());
}

/// Read a signed 16-bit value from the given input stream.
inline int16_t read16BitValue(InputStream &stream) {
    return static_cast<int16_t>((stream.read() << 8) | stream.read());
}

/// Read an unsigned 16-bit value from the given input stream.
inline uint16_t readUnsigned16BitValue(InputStream &stream) {
    return static_cast<uint16_t>(read16BitValue(stream));
}

/// Read a signed 32-bit value from the given input stream.
inline int32_t read32BitValue(InputStream &stream) {
    return stream.read() << 24 | stream.read() << 16 | stream.read() << 8 | stream.read();
}

/// Read an unsigned 32-bit value from the given input stream.
inline uint32_t readUnsigned32BitValue(InputStream &stream) {
    return static_cast<uint32_t>(read32BitValue(stream));
}

/// Write a signed 8-bit value to the given output stream.
inline bool write8BitValue(int8_t value, OutputStream &stream) {
    return stream.write(static_cast<uint8_t>(value)) == 1;
}

/// Write an unsigned 8-bit value to the given output stream.
inline bool writeUnsigned8BitValue(const uint8_t value, OutputStream &stream) {
    return stream.write(value) == 1;
}

/// Write a signed 16-bit value to the given output stream.
inline bool write16BitValue(const int16_t value, OutputStream &stream) {
    return stream.write(static_cast<uint8_t>(value >> 8)) &&
        stream.write(static_cast<uint8_t>(value));
}

/// Write an unsigned 16-bit value to the given output stream.
inline bool writeUnsigned16BitValue(const uint16_t value, OutputStream &stream) {
    return write16BitValue(static_cast<int16_t>(value), stream);
}

/// Write a signed 32-bit value to the given output stream.
inline bool write32BitValue(const int32_t value, OutputStream &stream) {
    return stream.write(static_cast<uint8_t>(value >> 24)) &&
        stream.write(static_cast<uint8_t>(value >> 16)) &&
        stream.write(static_cast<uint8_t>(value >> 8)) &&
        stream.write(static_cast<uint8_t>(value));
}

/// Write an unsigned 32-bit value to the given output stream.
inline bool writeUnsigned32BitValue(const uint32_t value, OutputStream &stream) {
    return write32BitValue(static_cast<int32_t>(value), stream);
}

}

}
}

#endif
