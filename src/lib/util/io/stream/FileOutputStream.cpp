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

#include "FileOutputStream.h"

#include "lib/interface.h"
#include "lib/util/base/Panic.h"
#include "lib/util/io/file/File.h"

namespace Util::Io {

FileOutputStream::FileOutputStream(const File &file) : FileOutputStream(file.getCanonicalPath()) {}

FileOutputStream::FileOutputStream(const String &path) {
    fileDescriptor = File::open(path);
    if (fileDescriptor < 0) {
        Util::Panic::fire(Panic::ILLEGAL_STATE, "FileOutputStream: Unable to open file!");
    }
}

FileOutputStream::FileOutputStream(int32_t fileDescriptor) : fileDescriptor(fileDescriptor) {}

bool FileOutputStream::write(uint8_t c) {
    return write(&c, 0, 1) == 1;
}

uint32_t FileOutputStream::write(const uint8_t *sourceBuffer, uint32_t offset, uint32_t length) {
    const auto written = writeFile(fileDescriptor, sourceBuffer + offset, pos, length);
    pos += written;

    return written;
}

void FileOutputStream::setPosition(uint32_t offset, File::SeekMode mode) {
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

uint32_t FileOutputStream::getPosition() const {
    return pos;
}

}
