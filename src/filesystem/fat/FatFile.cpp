/*
 * Copyright (C) 2018-2022 Heinrich-Heine-Universitaet Duesseldorf,
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

#include "FatFile.h"

#include "filesystem/fat/FatNode.h"
#include "lib/util/memory/String.h"

namespace Filesystem::Fat {

FatFile::FatFile(FIL *file, FILINFO *info) : FatNode(info), file(file) {}

FatFile::~FatFile() {
    delete file;
}

Util::File::Type FatFile::getFileType() {
    return Util::File::REGULAR;
}

Util::Data::Array<Util::Memory::String> FatFile::getChildren() {
    return Util::Data::Array<Util::Memory::String>(0);
}

uint64_t FatFile::readData(uint8_t *targetBuffer, uint64_t pos, uint64_t numBytes) {
    auto result = f_lseek(file, pos);
    if (result != FR_OK) {
        return 0;
    }

    uint32_t readBytes;
    result = f_read(file, targetBuffer, numBytes, &readBytes);
    if (result != FR_OK) {
        return 0;
    }

    return readBytes;
}

uint64_t FatFile::writeData(const uint8_t *sourceBuffer, uint64_t pos, uint64_t numBytes) {
    auto result = f_lseek(file, pos);
    if (result != FR_OK) {
        return 0;
    }

    uint32_t writtenBytes;
    result = f_write(file, sourceBuffer, numBytes, &writtenBytes);
    if (result != FR_OK) {
        return 0;
    }

    f_sync(file);
    return writtenBytes;
}

}