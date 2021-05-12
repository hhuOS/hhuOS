/*
 * Copyright (C) 2018-2021 Heinrich-Heine-Universitaet Duesseldorf,
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

#include <util/memory/Address.h>
#include "ByteArrayOutputStream.h"

namespace Util::Stream {

ByteArrayOutputStream::ByteArrayOutputStream() : ByteArrayOutputStream(DEFAULT_BUFFER_SIZE) {

}

ByteArrayOutputStream::ByteArrayOutputStream(uint32_t size) : buffer(new uint8_t[size]), size(size) {

}

ByteArrayOutputStream::~ByteArrayOutputStream() {
    delete[] buffer;
}

void ByteArrayOutputStream::getContent(uint8_t *target, uint32_t length) const {
    auto sourceAddress = Memory::Address<uint32_t>(buffer, size).add(position);
    auto targetAddress = Memory::Address<uint32_t>(target, length);

    targetAddress.copyRange(sourceAddress, position > length ? length : position);
}

uint32_t ByteArrayOutputStream::getSize() const {
    return position;
}

void ByteArrayOutputStream::reset() {
    position = 0;
}

void ByteArrayOutputStream::write(uint8_t c) {
    ensureRemainingCapacity(1);
    buffer[position++] = c;
}

void ByteArrayOutputStream::write(const uint8_t *source, uint32_t offset, uint32_t length) {
    if (offset < 0 || length < 0) {
        Device::Cpu::throwException(Device::Cpu::Exception::OUT_OF_BOUNDS, "ByteArrayOutputStream: Negative offset or size!");
    }

    ensureRemainingCapacity(length);
    auto sourceAddress = Memory::Address<uint32_t>(source, length + offset).add(offset);
    auto targetAddress = Memory::Address<uint32_t>(buffer, size).add(position);
    targetAddress.copyRange(sourceAddress, length);

    position += length;
}

void ByteArrayOutputStream::ensureRemainingCapacity(uint32_t count) {
    if (position + count < size) {
        return;
    }

    uint32_t newSize = size * 2;
    while (newSize < position + count) {
        newSize *= 2;
    }

    auto newBuffer = new uint8_t[newSize];
    Memory::Address<uint32_t> source(buffer, size);
    Memory::Address<uint32_t> target(newBuffer, newSize);
    target.copyRange(source, position);

    delete[] buffer;
    buffer = newBuffer;
    size = newSize;
}

}