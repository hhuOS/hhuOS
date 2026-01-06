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

#include "FatDirectory.h"

#include "lib/util/collection/ArrayList.h"
#include "filesystem/fat/FatNode.h"
#include "lib/util/base/String.h"
#include "lib/util/collection/Array.h"

namespace Filesystem::Fat {

FatDirectory::FatDirectory(const DIR &dir, const Util::String &path) : FatNode(path), directory(dir) {}

Util::Io::File::Type FatDirectory::getType() {
    return Util::Io::File::DIRECTORY;
}

Util::Array<Util::String> FatDirectory::getChildren() {
    if (getType() != Util::Io::File::DIRECTORY) {
        return Util::Array<Util::String>(0);
    }

    auto children = Util::ArrayList<Util::String>();
    auto *childInfo = new FILINFO{};

    while (true) {
        auto result = f_readdir(&directory, childInfo);
        if (result != FR_OK || childInfo->fname[0] == 0) {
            break;
        }

        children.add(childInfo->fname);
    }

    f_rewinddir(&directory);
    delete childInfo;
    return children.toArray();
}

uint64_t FatDirectory::readData([[maybe_unused]] uint8_t *targetBuffer, [[maybe_unused]] uint64_t pos, [[maybe_unused]] uint64_t numBytes) {
    return 0;
}

uint64_t FatDirectory::writeData([[maybe_unused]] const uint8_t *sourceBuffer, [[maybe_unused]] uint64_t pos, [[maybe_unused]] uint64_t numBytes) {
    return 0;
}

}