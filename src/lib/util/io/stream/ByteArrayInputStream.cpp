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

#include "ByteArrayInputStream.h"

namespace Util {
namespace Io {

int16_t ByteArrayInputStream::read() {
    if (checkBounds && position >= size) {
        // End of buffer reached
        return -1;
    }
	
    if (nullTerminated && buffer[position] == '\0') {
        return 0;
    }

    return buffer[position++];
}

int32_t ByteArrayInputStream::read(uint8_t *targetBuffer, const size_t offset, const size_t length) {
    size_t readBytes = 0;
    while (readBytes < length) {
        const auto byte = read();
        if (byte == -1) {
            if (readBytes == 0) {
                return -1;
            }

            break;
        }

        targetBuffer[offset + readBytes++] = byte;
    }

    return static_cast<int32_t>(readBytes);
}

int16_t ByteArrayInputStream::peek() {
    if (checkBounds && position >= size) {
        // End of buffer reached
        return -1;
    }

    return buffer[position];
}

}
}