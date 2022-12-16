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
#include "BufferedReader.h"
#include "lib/util/stream/Reader.h"

namespace Util::Stream {

BufferedReader::BufferedReader(Reader &reader, uint32_t size) : reader(reader), buffer(new char[size]), size(size) {}

BufferedReader::~BufferedReader() {
    delete[] buffer;
}

char BufferedReader::read() {
    char c = 0;
    int32_t count = read(&c, 0, 1);

    return count > 0 ? c : -1;
}

int32_t BufferedReader::read(char *targetBuffer, uint32_t length) {
    return read(targetBuffer, 0, length);
}

int32_t BufferedReader::read(char *targetBuffer, uint32_t offset, uint32_t length) {
    if (length == 0) {
        return 0;
    }

    if (position >= valid && !refill()) {
        return -1;
    }

    uint32_t ret = 0;

    do {
        uint32_t readCount = valid - position > length ? length : valid - position;
        Memory::Address<uint32_t> sourceAddress(buffer + position);
        Memory::Address<uint32_t> targetAddress(targetBuffer + offset);
        targetAddress.copyRange(sourceAddress, readCount);

        position += readCount;
        offset += readCount;
        length -= readCount;
        ret += readCount;
    } while (length > 0 && refill());

    return ret;
}

Memory::String BufferedReader::read(uint32_t length) {
    char *tmpBuffer = new char[length + 1];
    int32_t count = read(tmpBuffer, length);

    tmpBuffer[count] = 0;
    Memory::String ret = count <= 0 ? Memory::String() : Memory::String(tmpBuffer);

    delete[] tmpBuffer;
    return ret;
}

bool BufferedReader::refill() {
    if (position == valid) {
        position = 0;
        valid = 0;
    }

    int readCount = reader.read(buffer, valid, size - valid);

    if (readCount <= 0) {
        return false;
    }

    valid += readCount;
    return true;
}

Memory::String BufferedReader::readLine() {
    Util::Memory::String line;
    auto currentChar = reader.read();
    while (currentChar != -1 && currentChar != '\n') {
        line += currentChar;
        currentChar = reader.read();
    }

    return line;
}

}