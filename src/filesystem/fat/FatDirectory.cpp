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

#include "FatDirectory.h"

#include "lib/util/data/ArrayList.h"
#include "filesystem/fat/FatNode.h"
#include "lib/util/Exception.h"
#include "lib/util/data/Collection.h"
#include "lib/util/data/Iterator.h"
#include "lib/util/memory/String.h"

namespace Filesystem::Fat {

FatDirectory::FatDirectory(DIR *dir, FILINFO *info) : FatNode(info), directory(dir) {}

FatDirectory::~FatDirectory() {
    delete directory;
}

Util::File::Type FatDirectory::getFileType() {
    return Util::File::DIRECTORY;
}

Util::Data::Array<Util::Memory::String> FatDirectory::getChildren() {
    if (getFileType() != Util::File::DIRECTORY) {
        return Util::Data::Array<Util::Memory::String>(0);
    }

    auto children = Util::Data::ArrayList<Util::Memory::String>();
    auto *childInfo = new FILINFO{};

    while (true) {
        auto result = f_readdir(directory, childInfo);
        if (result != FR_OK || childInfo->fname[0] == 0) {
            break;
        }

        children.add(childInfo->fname);
    }

    f_rewinddir(directory);
    delete childInfo;
    return children.toArray();
}

uint64_t FatDirectory::readData(uint8_t *targetBuffer, uint64_t pos, uint64_t numBytes) {
    Util::Exception::throwException(Util::Exception::UNSUPPORTED_OPERATION, "FatDriver: Trying to read from a directory!");
}

uint64_t FatDirectory::writeData(const uint8_t *sourceBuffer, uint64_t pos, uint64_t numBytes) {
    Util::Exception::throwException(Util::Exception::UNSUPPORTED_OPERATION, "FatDriver: Trying to write to a directory!");
}

}