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
#include "FileStream.h"
#include "lib/util/base/Panic.h"
#include "lib/util/io/file/File.h"

namespace Util::Io {

FileInputStream::FileInputStream(const Io::File &file) : fileStream(static_cast<const char*>(file.getCanonicalPath()), FileStream::FileMode::READ) {
    if (fileStream.isError()) {
        Util::Panic::fire(Panic::INVALID_ARGUMENT, "FileInputStream: Unable to open file!");
    }
}

FileInputStream::FileInputStream(const String &path) : fileStream(static_cast<const char*>(path), FileStream::FileMode::READ) {
    if (fileStream.isError()) {
        Util::Panic::fire(Panic::INVALID_ARGUMENT, "FileInputStream: Unable to open file!");
    }
}

FileInputStream::FileInputStream(int32_t fileDescriptor) : fileStream(fileDescriptor, true, false) {
    if (fileStream.isError()) {
        Util::Panic::fire(Panic::INVALID_ARGUMENT, "FileInputStream: Unable to open file!");
    }
}

int16_t FileInputStream::read() {
    return fileStream.read();
}

int16_t FileInputStream::peek() {
	return fileStream.peek();
}

int32_t FileInputStream::read(uint8_t *targetBuffer, uint32_t offset, uint32_t length) {
    return fileStream.read(targetBuffer, offset, length);
}

bool FileInputStream::setAccessMode(File::AccessMode accessMode) const {
    return fileStream.setAccessMode(accessMode);
}

bool FileInputStream::isReadyToRead() {
    return fileStream.isReadyToRead();
}

}