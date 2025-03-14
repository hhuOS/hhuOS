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
#include "ByteArrayInputStream.h"

namespace Util::Io {

ByteArrayInputStream::ByteArrayInputStream(uint8_t *buffer, uint32_t size) : buffer(buffer), size(size) {}

int16_t ByteArrayInputStream::read() {
    if (position >= size && enforceSizeLimit) {
        return -1;
    }
	
	if (nullTerminated && buffer[position] == '\0') {
        return 0;
    }

    return buffer[position++];
}

int16_t ByteArrayInputStream::peek() {
	return buffer[position];
}

void ByteArrayInputStream::makeNullTerminated() {
	nullTerminated = true;
}

int32_t ByteArrayInputStream::read(uint8_t *targetBuffer, uint32_t offset, uint32_t length) {
    if (position >= size && enforceSizeLimit) {
        return -1;
    }

    uint32_t count = size - position > length ? length : size - position;
    auto sourceAddress = Address(buffer).add(position);
    auto targetAddress = Address(targetBuffer).add(offset);
    targetAddress.copyRange(sourceAddress, count);

    position += count;
    return count;
}

const uint8_t* ByteArrayInputStream::getBuffer() const {
    return buffer;
}

uint32_t ByteArrayInputStream::getLength() const {
    return size;
}

uint32_t ByteArrayInputStream::getPosition() const {
    return position;
}

uint32_t ByteArrayInputStream::getRemaining() const {
    return size - position;
}

bool ByteArrayInputStream::isEmpty() const {
    return size == 0;
}

void ByteArrayInputStream::disableSizeLimit() {
	enforceSizeLimit = false;
}

  bool ByteArrayInputStream::isReadyToRead() {
    return !isEmpty();
}

}