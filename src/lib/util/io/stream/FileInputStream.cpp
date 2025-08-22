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

#include <lib/interface.h>

#include "FileStream.h"
#include "lib/util/base/Panic.h"
#include "lib/util/io/file/File.h"

namespace Util::Io {

FileInputStream::FileInputStream(const File &file) : FileInputStream(file.getCanonicalPath()) {}

FileInputStream::FileInputStream(const String &path) {
    fileDescriptor = File::open(path);
    if (fileDescriptor < 0) {
        Util::Panic::fire(Panic::ILLEGAL_STATE, "FileOutputStream: Unable to open file!");
    }
}

FileInputStream::FileInputStream(int32_t fileDescriptor) : fileDescriptor(fileDescriptor) {}

int16_t FileInputStream::read() {
    const auto pushedBack = Async::Atomic<int16_t>(pushedBackByte).getAndSet(-1);
    if (pushedBack >= 0) {
        return pushedBack;
    }

    uint8_t byte;
    return read(&byte, 0, 1) == 1 ? byte : -1;
}

int16_t FileInputStream::peek() {
    const int16_t byte = read();
    if (byte >= 0) {
        pushBack(byte);
    }

    return byte;
}

int32_t FileInputStream::read(uint8_t *targetBuffer, uint32_t offset, uint32_t length) {
    bool readPushedBack = false;

    const auto pushedBack = Async::Atomic<int16_t>(pushedBackByte).getAndSet(-1);
    if (pushedBack >= 0) {
        targetBuffer[offset] = static_cast<uint8_t>(pushedBack);
        readPushedBack = true;
    }

    const auto read = readFile(fileDescriptor, targetBuffer, pos, length - (readPushedBack ? 1 : 0));
    pos += read;

    return static_cast<int32_t>(read + (readPushedBack ? 1 : 0));
}

bool FileInputStream::isReadyToRead() {
    if (Async::Atomic<int16_t>(pushedBackByte).get() >= 0) {
        return true;
    }

    return File::isReadyToRead(fileDescriptor);
}

void FileInputStream::setPosition(uint32_t offset, File::SeekMode mode) {
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
}

uint32_t FileInputStream::getPosition() const {
    return pos;
}

bool FileInputStream::pushBack(uint8_t c) {
    return Async::Atomic<int16_t>(pushedBackByte).compareAndSet(-1, c);
}

bool FileInputStream::setAccessMode(File::AccessMode mode) {
    return File::setAccessMode(fileDescriptor, mode);
}

}
