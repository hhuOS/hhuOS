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

#include "lib/util/memory/Address.h"
#include "lib/util/Exception.h"
#include "BufferedOutputStream.h"
#include "lib/util/stream/FilterOutputStream.h"

namespace Util {
namespace Stream {
class OutputStream;
}  // namespace Stream
}  // namespace Util

namespace Util::Stream {

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
    if (offset < 0 || length < 0) {
        Exception::throwException(Exception::OUT_OF_BOUNDS, "BufferedOutputStream: Negative offset or size!");
    }

    if (length < (size - position)) {
        Memory::Address<uint32_t> sourceAddress(sourceBuffer + offset);
        Memory::Address<uint32_t> targetAddress(buffer + position);

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