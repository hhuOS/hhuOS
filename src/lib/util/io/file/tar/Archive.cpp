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

#include "lib/util/base/Address.h"
#include "Archive.h"

namespace Util::Io::Tar {

Archive::Archive(uint32_t address) {
    auto archiveAddress = Util::Address(address);

    while (true) {
        auto *header = reinterpret_cast<Header*>(archiveAddress.get());
        if (header->filename[0] == '\0') {
            break;
        }

        uint32_t size = calculateFileSize(*header);
        totalSize += size;
        headers.add(header);

        if (header->typeFlag == LF_OLDNORMAL) {
            fileCount++;
        }

        archiveAddress = archiveAddress.add(((size / BLOCKSIZE) + 1) * BLOCKSIZE);
        if (size % 512) {
            archiveAddress = archiveAddress.add(512);
        }
    }
}

uint32_t Archive::calculateFileSize(const Header &header) {
    uint32_t ret = 0;
    uint32_t count = 1;

    for (uint32_t i = 11; i > 0; i--, count *= 8) {
        ret += ((header.size[i - 1] - '0') * count);
    }

    return ret;
}

Util::Array<Archive::Header> Archive::getFileHeaders() {
    Util::Array<Header> fileHeaders(fileCount);
    uint32_t arrayIndex = 0;

    Header *header;
    for (uint32_t i = 0; i < headers.size(); i++) {
        header = headers.get(i);
        if (header->typeFlag == LF_OLDNORMAL) {
            fileHeaders[arrayIndex] = *header;
            arrayIndex++;
        }
    }

    return fileHeaders;
}

uint8_t *Archive::getFile(const Util::String &path) {
    for (auto *header : headers) {
        if (path == header->filename) {
            return reinterpret_cast<uint8_t*>(header) + BLOCKSIZE;
        }
    }

    return nullptr;
}

}



