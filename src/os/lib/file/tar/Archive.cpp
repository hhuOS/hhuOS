/*
 * Copyright (C) 2018  Filip Krakowski
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "Archive.h"

namespace Tar {

    uint32_t Archive::calculateSize(const uint8_t *size) {

        uint32_t ret = 0;

        uint32_t i = 0;

        uint32_t count = 1;

        for (i = 11; i > 0; i--, count *= 8) {

            ret += ((size[i - 1] - '0') * count);
        }

        return ret;
    }

    Archive &Archive::from(Address &address) {

        Archive *tar = new Archive();

        uint32_t i;

        uint32_t addr = (uint32_t) address;

        for (i = 0; ; i++) {

            Header *header = (Header*) addr;

            if (header->filename[0] == '\0') {

                break;
            }

            uint32_t size = calculateSize(header->size);

            tar->totalSize += size;

            tar->headers.add(header);

            if (header->typeFlag == LF_OLDNORMAL) {

                tar->fileCount++;
            }

            addr += ((size / BLOCKSIZE) + 1) * BLOCKSIZE;

            if (size % 512) {

                addr += 512;
            }
        }

        tar->fileCount = i;

        return *tar;
    }


    Archive &Archive::from(File &file) {
        char *buffer = new char[file.getLength()];

        file.readBytes(buffer, file.getLength());

        Address address((uint32_t) buffer);

        return from(address);
    }

    Util::Array<Header*> Archive::getFileHeaders() {

        Util::Array<Header*> fileHeaders(fileCount);

        uint32_t arrayIndex = 0;

        Header *header;

        for (uint32_t i = 0; i < headers.size(); i++) {

            header = headers.get(i);

            if (header->typeFlag == LF_OLDNORMAL) {

                fileHeaders[arrayIndex] = header;

                arrayIndex++;
            }
        }

        return fileHeaders;
    }

    uint8_t *Archive::getFile(const String &path) {

        for (auto &header : getFileHeaders()) {

            if (path == header->filename) {

                return ((uint8_t*) header) + BLOCKSIZE;
            }
        }

        return nullptr;
    }
}



