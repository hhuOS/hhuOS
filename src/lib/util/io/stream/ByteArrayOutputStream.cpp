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

#include "ByteArrayOutputStream.h"

#include "lib/util/base/Address.h"

namespace Util::Io {

ByteArrayOutputStream::ByteArrayOutputStream(const size_t size) :
    buffer(new uint8_t[size]), size(size) {}

ByteArrayOutputStream::ByteArrayOutputStream(uint8_t *buffer) :
    buffer(buffer), allocatedBuffer(false), checkBounds(false) {}

ByteArrayOutputStream::ByteArrayOutputStream(uint8_t *buffer, const size_t size) :
    buffer(buffer), size(size), allocatedBuffer(false) {}

ByteArrayOutputStream::~ByteArrayOutputStream() {
    if (allocatedBuffer) {
        delete[] buffer;
    }
}

bool ByteArrayOutputStream::write(const uint8_t byte) {
    if (ensureRemainingCapacity(1)) {
        buffer[position++] = byte;
        return true;
    }

    return false;
}

size_t ByteArrayOutputStream::write(const uint8_t *sourceBuffer, const size_t offset, const size_t length) {
    const auto toWrite = ensureRemainingCapacity(length);

    const auto sourceAddress = Address(sourceBuffer).add(offset);
    const auto targetAddress = Address(buffer).add(position);
    targetAddress.copyRange(sourceAddress, toWrite);

    position += toWrite;
    return toWrite;
}

void ByteArrayOutputStream::getContent(uint8_t *target, const size_t length) const {
    const auto sourceAddress = Address(buffer);
    const auto targetAddress = Address(target);

    targetAddress.copyRange(sourceAddress, position > length ? length : position);
}

String ByteArrayOutputStream::getContent() const {
    return String(buffer, position);
}

size_t ByteArrayOutputStream::getPosition() const {
    return position;
}

bool ByteArrayOutputStream::isEmpty() const {
    return position == 0;
}

void ByteArrayOutputStream::reset() {
    position = 0;
}

size_t ByteArrayOutputStream::ensureRemainingCapacity(const size_t count) {
    if (!checkBounds) {
        // Bounds checking disabled -> Capacity is always sufficient
        return count;
    }

    if (position + count < size) {
        // Enough space available -> Nothing to do
        return count;
    }

    if (!allocatedBuffer) {
        // We do not own the buffer and cannot resize it -> Return remaining space
        return size - position;
    }

    // Allocate a new buffer, doubling the size until it is large enough
    size_t newSize = size * 2;
    while (newSize < position + count) {
        newSize *= 2;
    }

    const auto newBuffer = new uint8_t[newSize];
    Address(newBuffer).copyRange(buffer, position);

    delete[] buffer;
    buffer = newBuffer;
    size = newSize;

	return count;
}

uint8_t* ByteArrayOutputStream::getBuffer() const {
    return buffer;
}

}