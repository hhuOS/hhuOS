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

#ifndef HHUOS_LIB_UTIL_STREAM_NUMBERUTIL_H
#define HHUOS_LIB_UTIL_STREAM_NUMBERUTIL_H

#include <stdint.h>

#include "io/stream/InputStream.h"
#include "io/stream/OutputStream.h"

/// Provides utility functions for reading numbers from input streams and writing numbers to output streams.
/// The streams are expected to provide raw bytes. For parsing numbers from text streams, use a `ScanStream`.
/// These functions are used a lot in the network stack, to parse network headers and write network packets.
namespace Util::Io::NumberUtil {

/// Read a signed 8-bit value from the given input stream.
[[nodiscard]] int8_t read8BitValue(InputStream &stream);

/// Read an unsigned 8-bit value from the given input stream.
[[nodiscard]] uint8_t readUnsigned8BitValue(InputStream &stream);

/// Read a signed 16-bit value from the given input stream.
[[nodiscard]] int16_t read16BitValue(InputStream &stream);

/// Read an unsigned 16-bit value from the given input stream.
[[nodiscard]] uint16_t readUnsigned16BitValue(InputStream &stream);

/// Read a signed 32-bit value from the given input stream.
[[nodiscard]] int32_t read32BitValue(InputStream &stream);

/// Read an unsigned 32-bit value from the given input stream.
[[nodiscard]] uint32_t readUnsigned32BitValue(InputStream &stream);

/// Write a signed 8-bit value to the given output stream.
void write8BitValue(int8_t value, OutputStream &stream);

/// Write an unsigned 8-bit value to the given output stream.
void writeUnsigned8BitValue(uint8_t value, OutputStream &stream);

/// Write a signed 16-bit value to the given output stream.
void write16BitValue(int16_t value, OutputStream &stream);

/// Write an unsigned 16-bit value to the given output stream.
void writeUnsigned16BitValue(uint16_t value, OutputStream &stream);

/// Write a signed 32-bit value to the given output stream.
void write32BitValue(int32_t value, OutputStream &stream);

/// Write an unsigned 32-bit value to the given output stream.
void writeUnsigned32BitValue(uint32_t value, OutputStream &stream);

}

#endif
