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

#include "lib/util/base/Address.h"
#include "BufferedInputStream.h"
#include "lib/util/io/stream/FilterInputStream.h"

namespace Util {
namespace Io {
class InputStream;
}  // namespace Stream
}  // namespace Util

namespace Util::Io {

BufferedInputStream::BufferedInputStream(InputStream &stream) : BufferedInputStream(stream, DEFAULT_BUFFER_SIZE) {}

BufferedInputStream::BufferedInputStream(InputStream &stream, uint32_t size) : FilterInputStream(stream), buffer(new uint8_t[size]), size(size) {}

BufferedInputStream::~BufferedInputStream() {
    delete[] buffer;
}

int16_t BufferedInputStream::read() {
    if (position >= valid && !refill()) {
        return -1;
    }

    return buffer[position++];
}

int16_t BufferedInputStream::peek() {
	if (position >= valid && !refill()) {
        return -1;
    }
	
	return buffer[position];
}

int32_t BufferedInputStream::read(uint8_t *target, uint32_t offset, uint32_t length) {
    if (length == 0) {
        return 0;
    }

    if (position >= valid && !refill()) {
        return -1;
    }

    uint32_t ret = 0;

    do {
        uint32_t readCount = valid - position > length ? length : valid - position;
        Address sourceAddress(buffer + position);
        Address targetAddress(target + offset);
        targetAddress.copyRange(sourceAddress, readCount);

        position += readCount;
        offset += readCount;
        ret += readCount;
        length -= readCount;
    } while (length > 0 && refill());

    return ret;
}

bool BufferedInputStream::refill() {
    if (position == valid) {
        position = 0;
        valid = 0;
    }

    int readCount = FilterInputStream::read(buffer, valid, size - valid);

    if (readCount <= 0) {
        return false;
    }

    valid += readCount;
    return true;
}

bool BufferedInputStream::isReadyToRead() {
    return valid > position || refill();
}

}