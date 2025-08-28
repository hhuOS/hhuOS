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

#include "BufferedInputStream.h"

#include "base/Address.h"
#include "io/stream/FilterInputStream.h"

namespace Util::Io {

BufferedInputStream::BufferedInputStream(InputStream &stream, const size_t bufferSize) :
    FilterInputStream(stream), buffer(new uint8_t[bufferSize]), size(bufferSize) {}

BufferedInputStream::~BufferedInputStream() {
    delete[] buffer;
}

int16_t BufferedInputStream::read() {
    if (position >= valid && !refill()) {
        return -1;
    }

    return buffer[position++];
}

int32_t BufferedInputStream::read(uint8_t *target, size_t offset, size_t length) {
    if (length == 0) {
        return 0;
    }

    if (position >= valid && !refill()) {
        return -1;
    }

    size_t ret = 0;

    // Keep reading until we have read the requested length or there is no more data available.
    // If the internal buffer is exhausted, try to refill it with every iteration.
    // If refilling fails, the underlying stream has reached the end or an error occurred,
    // and we stop reading.
    do {
        // Copy as many bytes as possible from the internal buffer to the target buffer
        const auto readCount = valid - position > length ? length : valid - position;
        const auto sourceAddress = Address(buffer).add(position);
        const auto targetAddress = Address(target).add(offset);
        targetAddress.copyRange(sourceAddress, readCount);

        // Update positions and counters
        position += readCount;
        offset += readCount;
        ret += readCount;
        length -= readCount;
    } while (length > 0 && refill());

    return static_cast<int32_t>(ret);
}

int16_t BufferedInputStream::peek() {
    if (position >= valid && !refill()) {
        return -1;
    }

    return buffer[position];
}

bool BufferedInputStream::isReadyToRead() {
    return valid > position || FilterInputStream::isReadyToRead();
}

void BufferedInputStream::clearBuffer() {
    position = 0;
    valid = 0;
}

bool BufferedInputStream::refill() {
    // Check if the buffer is fully exhausted and reset the position and valid counters if so.
    if (position == valid) {
        position = 0;
        valid = 0;
    }

    // Try to fill the internal buffer with data from the underlying stream.
    const auto readCount = FilterInputStream::read(buffer, valid, size - valid);
    valid += readCount;

    return readCount > 0;
}

}