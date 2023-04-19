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

#include "lib/interface.h"
#include "FileInputStream.h"
#include "lib/util/base/Exception.h"
#include "lib/util/io/file/File.h"

namespace Util::Io {

FileInputStream::FileInputStream(const Io::File &file) : fileDescriptor(Io::File::open(file.getCanonicalPath())) {
    if (fileDescriptor < 0) {
        Util::Exception::throwException(Exception::INVALID_ARGUMENT, "FileInputStream: Unable to open file!");
    }
}

FileInputStream::FileInputStream(const String &path) : fileDescriptor(Io::File::open(path)) {
    if (fileDescriptor < 0) {
        Util::Exception::throwException(Exception::INVALID_ARGUMENT, "FileInputStream: Unable to open file!");
    }
}

FileInputStream::FileInputStream(int32_t fileDescriptor) : fileDescriptor(fileDescriptor) {
    if (fileDescriptor < 0) {
        Util::Exception::throwException(Exception::INVALID_ARGUMENT, "FileInputStream: Unable to open file!");
    }
}

FileInputStream::~FileInputStream() {
    Io::File::close(fileDescriptor);
}

int16_t FileInputStream::read() {
    uint8_t c;
    int32_t count = read(&c, 0, 1);

    return count > 0 ? c : -1;
}

int32_t FileInputStream::read(uint8_t *targetBuffer, uint32_t offset, uint32_t length) {
    auto fileType = getFileType(fileDescriptor);
    auto fileLength = getFileLength(fileDescriptor);

    if (fileType == Io::File::REGULAR && pos >= fileLength) {
        return -1;
    }

    uint32_t count = readFile(fileDescriptor, targetBuffer + offset, pos, length);
    pos += count;

    return count > 0 ? count : -1;
}

}