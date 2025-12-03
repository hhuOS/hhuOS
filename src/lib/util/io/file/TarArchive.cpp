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

#include "TarArchive.h"

#include "util/base/Address.h"
#include "util/collection/ArrayList.h"
#include "util/io/stream/FileInputStream.h"

namespace Util {
namespace Io {

TarArchive::TarArchive(uint8_t *buffer) : deleteArchiveBuffer(false), archiveBuffer(buffer) {
    parseArchive();
}

TarArchive::TarArchive(const File &file) : deleteArchiveBuffer(true), archiveBuffer(new uint8_t[file.getLength()]) {
    FileInputStream inputStream(file);
    inputStream.read(archiveBuffer, 0, file.getLength());

    parseArchive();
}

TarArchive::~TarArchive() {
    if (deleteArchiveBuffer) {
        delete[] archiveBuffer;
    }
}

const TarArchive::Header* TarArchive::getHeader(const String &path) const {
    for (auto *header : headers) {
        if (path == header->filename) {
            return header;
        }
    }

    return nullptr;
}

void TarArchive::parseArchive() {
    ArrayList<const Header*> headerList;
    auto archiveAddress = Address(archiveBuffer);

    while (true) {
        const auto *header = reinterpret_cast<Header*>(archiveAddress.get());

        // End of archive reached
        if (header->filename[0] == '\0') {
            break;
        }

        // If the checksum is invalid, stop parsing the archive
        if (header->calculateChecksum() != header->parseChecksum()) {
            break;
        }

        // We only support normal files
        if (header->type == OLDNORMAL) {
            headerList.add(header);
        }

        // Move to the next header
        const auto size = header->parseSize();
        archiveAddress = archiveAddress.add((size / sizeof(Header) + 1) * sizeof(Header));
        if (archiveAddress.get() % 512 != 0) {
            archiveAddress = archiveAddress.add(512);
        }
    }

    headers = headerList.toArray();
}

size_t TarArchive::parseNumber(const char *string, const size_t length) {
    size_t ret = 0;
    size_t count = 1;

    // Skip trailing spaces and null bytes
    auto i = static_cast<int32_t>(length - 1);
    while (i >= 0 && (string[i] == ' ' || string[i] == '\0')) {
        i--;
    }

    // Parse the number char by char in reverse order
    while (i >= 0) {
        ret += (string[i] - '0') * count;

        count *= 8;
        i--;
    }

    return ret;
}

}

}