/*
 * Copyright (C) 2017-2026 Heinrich Heine University Düsseldorf,
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

#include "InputStream.h"

namespace Util {
namespace Io {

String InputStream::readString(const size_t length) {
    auto *stringBuffer = new uint8_t[length];
    const auto count = read(stringBuffer, 0, length);

    const auto string = count <= 0 ? String() : String(stringBuffer, length);

    delete[] stringBuffer;
    return string;
}

InputStream::Line InputStream::readLine() {
    String line;
    auto currentChar = read();
    while (currentChar != -1 && currentChar != '\n') {
        line += static_cast<char>(currentChar);
        currentChar = read();
    }

    return Line{line, line.isEmpty() && currentChar == -1};
}

size_t InputStream::skip(const size_t amount) {
    if (skipBuffer == nullptr) {
        skipBuffer = new uint8_t[SKIP_BUFFER_SIZE];
    }

    auto remaining = amount;

    while (remaining > 0) {
        const auto toRead = SKIP_BUFFER_SIZE > remaining ? remaining : SKIP_BUFFER_SIZE;
        const auto skipped = read(skipBuffer, 0, toRead);
        if (skipped < 0) {
            break;
        }

        remaining -= skipped;
    }

    return amount - remaining;
}

}
}