/*
 * Copyright (C) 2018-2024 Heinrich-Heine-Universitaet Duesseldorf,
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

#include "lib/util/base/Address.h"
#include "BufferedOutputStream.h"
#include "lib/util/io/stream/FilterOutputStream.h"

namespace Util {
namespace Io {
class OutputStream;
}  // namespace Stream
}  // namespace Util

namespace Util::Io {

BufferedOutputStream::BufferedOutputStream(OutputStream &stream, uint32_t size): FilterOutputStream(stream), buffer(new uint8_t[size]), size(size) {}

BufferedOutputStream::~BufferedOutputStream() {
    delete[] buffer;
}

void BufferedOutputStream::write(uint8_t c) {
    if (position == size) {
        flush();
    }

    buffer[position++] = c;
}

void BufferedOutputStream::write(const uint8_t *sourceBuffer, uint32_t offset, uint32_t length) {
    if (length < (size - position)) {
        Address<uint32_t> sourceAddress(sourceBuffer + offset);
        Address<uint32_t> targetAddress(buffer + position);

        targetAddress.copyRange(sourceAddress, length);
        position += length;
    } else {
        flush();
        FilterOutputStream::write(sourceBuffer, offset, length);
    }
}

void BufferedOutputStream::flush() {
    FilterOutputStream::write(buffer, 0, position);
    position = 0;
    FilterOutputStream::flush();
}

}