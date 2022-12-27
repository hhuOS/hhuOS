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
#include "ByteArrayInputStream.h"

namespace Util::Stream {

ByteArrayInputStream::ByteArrayInputStream(uint8_t *buffer, uint32_t size, bool deleteBuffer) : buffer(buffer), size(size), deleteBuffer(deleteBuffer) {}

ByteArrayInputStream::ByteArrayInputStream(Network::Datagram &datagram) : ByteArrayInputStream(datagram.getData(), datagram.getLength(), false) {}

ByteArrayInputStream::~ByteArrayInputStream() {
    if (deleteBuffer) {
        delete[] buffer;
    }
}

int16_t ByteArrayInputStream::read() {
    if (position >= size) {
        return -1;
    }

    return buffer[position++];
}

int32_t ByteArrayInputStream::read(uint8_t *targetBuffer, uint32_t offset, uint32_t length) {
    if (position >= size) {
        return -1;
    }

    uint32_t count = size - position > length ? length : size - position;
    auto sourceAddress = Memory::Address<uint32_t>(buffer).add(position);
    auto targetAddress = Memory::Address<uint32_t>(targetBuffer).add(offset);
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

}