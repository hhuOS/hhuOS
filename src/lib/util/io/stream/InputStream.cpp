/*
 * Copyright (C) 2018-2023 Heinrich-Heine-Universitaet Duesseldorf,
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

#include "InputStream.h"

namespace Util::Io {

String InputStream::readString(uint32_t length) {
    auto *tmpBuffer = new uint8_t[length];
    int32_t count = read(tmpBuffer, 0, length);

    String ret = count <= 0 ? String() : String(tmpBuffer, length);

    delete[] tmpBuffer;
    return ret;
}

String InputStream::readLine() {
    Util::String line;
    auto currentChar = read();
    while (currentChar != -1 && currentChar != '\n') {
        line += static_cast<char>(currentChar);
        currentChar = read();
    }

    return line;
}

uint32_t InputStream::skip(uint32_t amount) {
    if (amount <= 0) {
        return 0;
    }

    uint32_t remaining = amount;
    uint32_t bufferSize = amount > SKIP_BUFFER_SIZE ? SKIP_BUFFER_SIZE : amount;
    auto *buffer = new uint8_t[bufferSize];

    while (remaining > 0) {
        int32_t skipped = read(buffer, 0, bufferSize > remaining ? remaining : bufferSize);
        if (skipped <= 0) {
            break;
        }

        remaining -= skipped;
    }

    delete[] buffer;
    return amount - remaining;
}

}