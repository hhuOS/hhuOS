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
 */

#include "BufferedOutputStream.h"

#include "util/base/Address.h"
#include "util/io/stream/FilterOutputStream.h"

namespace Util::Io {

BufferedOutputStream::BufferedOutputStream(OutputStream &stream, const size_t size):
    FilterOutputStream(stream), buffer(new uint8_t[size]), size(size) {}

BufferedOutputStream::~BufferedOutputStream() {
    flush();
    delete[] buffer;
}

bool BufferedOutputStream::write(const uint8_t byte) {
    // Check if the buffer is full and flush it if necessary
    if (position == size) {
        if (flush() == 0) {
            // Flushing failed, likely due to an error in the underlying stream
            return false;
        }
    }

    // Add the byte to the buffer
    buffer[position++] = byte;
    return true;
}

uint32_t BufferedOutputStream::write(const uint8_t *sourceBuffer, const size_t offset, const size_t length) {
    // If the data fits into the remaining buffer space, copy it into the buffer
    if (length < size - position) {
        const auto sourceAddress = Address(sourceBuffer).add(offset);
        const auto targetAddress = Address(buffer).add(position);

        targetAddress.copyRange(sourceAddress, length);
        position += length;

        return length;
    }

    // If the data is larger than the buffer, flush the buffer and write directly to the underlying stream
    flush();
    return FilterOutputStream::write(sourceBuffer, offset, length);
}

uint32_t BufferedOutputStream::flush() {
    const auto flushed = FilterOutputStream::write(buffer, 0, position);
    position -= flushed;
    FilterOutputStream::flush();

    return flushed;
}

}