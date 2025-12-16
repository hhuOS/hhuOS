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

#include "FileInputStream.h"

#include "interface.h"
#include "util/base/Panic.h"
#include "util/io/file/File.h"

namespace Util {
namespace Io {

FileInputStream::FileInputStream(const String &path) {
    fileDescriptor = File::open(path);
    if (fileDescriptor < 0) {
        Util::Panic::fire(Panic::ILLEGAL_STATE, "FileOutputStream: Unable to open file!");
    }
}

FileInputStream::~FileInputStream() {
    if (closeFileDescriptor) {
        File::close(fileDescriptor);
    }
}

int16_t FileInputStream::read() {
    if (peekedChar >= 0) {
        // A previous peek operation has already read a byte -> Return it instead of reading a new one
        const auto byte = peekedChar;
        peekedChar = -1;

        return byte;
    }

    uint8_t byte;
    if (read(&byte, 0, 1)) {
        return byte;
    }

    return -1;
}

int32_t FileInputStream::read(uint8_t *targetBuffer, size_t offset, size_t length) {
    size_t peeked = 0;
    if (peekedChar >= 0) {
        // A previous peek operation has already read a byte -> Use it first before reading more bytes
        targetBuffer[offset] = peekedChar;
        peekedChar = -1;
        peeked = 1;

        if (length == 1) {
            // Only one byte was requested, which was already provided by the peeked character
            return 1;
        }
    }

    // Read the remaining bytes from the file
    const auto read = readFile(fileDescriptor, targetBuffer + offset + peeked, pos, length - peeked);
    pos += read;

    return static_cast<int32_t>(read + peeked);
}

int16_t FileInputStream::peek() {
    if (peekedChar >= 0) {
        // A previous peek operation has already read a byte -> Return it instead of reading a new one
        return peekedChar;
    }

    /// Read a byte from the file and store it for future peek() or read() calls
    peekedChar = read();
    return peekedChar;
}

bool FileInputStream::isReadyToRead() {
    if (peekedChar >= 0) {
        // A previous peek operation has already read a byte -> We are ready to read at least that byte
        return true;
    }

    // Check if the underlying file descriptor has data available to read
    return File::isReadyToRead(fileDescriptor);
}

void FileInputStream::setPosition(const int64_t offset, const File::SeekMode mode) {
    switch (mode) {
        case File::SeekMode::SET:
            pos = offset;
            break;
        case File::SeekMode::CURRENT:
            pos += offset;
            break;
        case File::SeekMode::END:
            pos = getFileLength(fileDescriptor) - offset;
            break;
    }

    peekedChar = -1;
}

}
}
