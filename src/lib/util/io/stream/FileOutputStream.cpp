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
#include "FileOutputStream.h"
#include "lib/util/base/Exception.h"
#include "lib/util/io/file/File.h"

namespace Util::Io {

FileOutputStream::FileOutputStream(const Io::File &file) : fileDescriptor(Io::File::open(file.getCanonicalPath())) {
    if (fileDescriptor < 0) {
        Util::Exception::throwException(Exception::ILLEGAL_STATE, "FileReader: Unable to open file!");
    }
}

FileOutputStream::FileOutputStream(const String &path) : fileDescriptor(Io::File::open(path)) {
    if (fileDescriptor < 0) {
        Util::Exception::throwException(Exception::ILLEGAL_STATE, "FileReader: Unable to open file!");
    }
}

FileOutputStream::FileOutputStream(int32_t fileDescriptor) : fileDescriptor(fileDescriptor) {
    if (fileDescriptor < 0) {
        Util::Exception::throwException(Exception::ILLEGAL_STATE, "FileReader: Unable to open file!");
    }
}

FileOutputStream::~FileOutputStream() {
    Io::File::close(fileDescriptor);
}

void FileOutputStream::write(uint8_t c) {
    write(&c, 0, 1);
}

void FileOutputStream::write(const uint8_t *sourceBuffer, uint32_t offset, uint32_t length) {
    uint32_t count = writeFile(fileDescriptor, sourceBuffer + offset, pos, length);
    pos += count;
}

}