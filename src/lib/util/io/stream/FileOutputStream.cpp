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
#include "lib/util/base/Panic.h"
#include "lib/util/io/file/File.h"
#include "lib/util/io/stream/FileStream.h"

namespace Util::Io {

FileOutputStream::FileOutputStream(const Io::File &file) : fileStream(static_cast<const char*>(file.getCanonicalPath()), FileStream::FileMode::WRITE) {
    if (fileStream.isError()) {
        Util::Panic::fire(Panic::ILLEGAL_STATE, "FileOutputStream: Unable to open file!");
    }
}

FileOutputStream::FileOutputStream(const String &path) : fileStream(static_cast<const char*>(path), FileStream::FileMode::WRITE) {
    if (fileStream.isError()) {
        Util::Panic::fire(Panic::ILLEGAL_STATE, "FileOutputStream: Unable to open file!");
    }
}

FileOutputStream::FileOutputStream(int32_t fileDescriptor) : fileStream(fileDescriptor, false, true) {
    if (fileStream.isError()) {
        Util::Panic::fire(Panic::ILLEGAL_STATE, "FileOutputStream: Unable to open file!");
    }
}

void FileOutputStream::write(uint8_t c) {
    fileStream.write(c);
}

void FileOutputStream::write(const uint8_t *sourceBuffer, uint32_t offset, uint32_t length) {
    fileStream.write(sourceBuffer, offset, length);
}

}