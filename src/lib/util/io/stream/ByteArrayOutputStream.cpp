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
#include "ByteArrayOutputStream.h"

namespace Util::Io {

ByteArrayOutputStream::ByteArrayOutputStream() : ByteArrayOutputStream(DEFAULT_BUFFER_SIZE) {}

ByteArrayOutputStream::ByteArrayOutputStream(uint32_t size) : buffer(new uint8_t[size]), size(size) {}

ByteArrayOutputStream::ByteArrayOutputStream(uint8_t * buffer, uint32_t size) : buffer(buffer), size(size), allocatedBuffer(false) {
}

ByteArrayOutputStream::~ByteArrayOutputStream() {
    if (allocatedBuffer) delete[] buffer;
}

void ByteArrayOutputStream::getContent(uint8_t *target, uint32_t length) const {
    auto sourceAddress = Address<uint32_t>(buffer);
    auto targetAddress = Address<uint32_t>(target);

    targetAddress.copyRange(sourceAddress, size > length ? length : size);
}

String ByteArrayOutputStream::getContent() const {
    return Util::String(buffer, position);
}

uint32_t ByteArrayOutputStream::getLength() const {
    return position;
}

bool ByteArrayOutputStream::isEmpty() const {
    return position == 0;
}

void ByteArrayOutputStream::reset() {
    position = 0;
}

bool ByteArrayOutputStream::sizeLimitReached() {
	return enforceSizeLimit && position == size;
}

void ByteArrayOutputStream::setEnforceSizeLimit(bool value) {
	enforceSizeLimit = value;
}

void ByteArrayOutputStream::write(uint8_t c) {
    if (ensureRemainingCapacity(1)) {
        buffer[position++] = c;
    }
}

void ByteArrayOutputStream::write(const uint8_t *sourceBuffer, uint32_t offset, uint32_t length) {
    length = ensureRemainingCapacity(length);
    auto sourceAddress = Address<uint32_t>(sourceBuffer).add(offset);
    auto targetAddress = Address<uint32_t>(buffer).add(position);
    targetAddress.copyRange(sourceAddress, length);

    position += length;
}

uint32_t ByteArrayOutputStream::ensureRemainingCapacity(uint32_t count) {
    if (position + count < size) {
        return count;
    }
	
	if (enforceSizeLimit) return size - position;
	
	if (!enforceSizeLimit && !allocatedBuffer) return count; //if no size limits are enforced on remote buffer, writes beyond end are allowed

    uint32_t newSize = size * 2;
    while (newSize < position + count) {
        newSize *= 2;
    }

    auto newBuffer = new uint8_t[newSize];
    Address<uint32_t> source(buffer);
    Address<uint32_t> target(newBuffer);
    target.copyRange(source, position);

    delete[] buffer;
    buffer = newBuffer;
    size = newSize;
	
	return count;
}

uint8_t* ByteArrayOutputStream::getBuffer() const {
    return buffer;
}

uint32_t ByteArrayOutputStream::getPosition() const {
    return position;
}

}