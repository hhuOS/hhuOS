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

#ifndef HHUOS_LIB_UTIL_IO_TARARCHIVE_H
#define HHUOS_LIB_UTIL_IO_TARARCHIVE_H

#include <stddef.h>
#include <stdint.h>

#include "util/base/String.h"
#include "util/collection/Array.h"
#include "util/io/file/File.h"

namespace Util {
namespace Io {

/// Class to parse tar archives and extract files from it.
/// This implementation only supports the most basic features of the tar format.
/// It allows to read files from an uncompressed tar archive, but does not care
/// about any metadata like user id, group id, permissions, etc.
/// While parsing the archive, the checksum of each header is verified to ensure the integrity of the archive.
/// If a checksum does not match, parsing stops and no further files are available.
class TarArchive {

public:
    /// The header of a file within a tar archive.
    /// All fields are stored as ASCII-strings.
    /// Numeric values are stored as octal numbers in ASCII-string format.
    struct Header {
        /// The name of the file (ASCII-string).
        char filename[100];
        /// The file's mode (e.g. permissions).
        char mode[8];
        /// The owner's user id.
        char userId[8];
        /// The owner's group id.
        char groupId[8];
        /// The filesize in bytes.
        char size[12];
        /// The file's last modification time (timestamp).
        char modificationTime[12];
        /// The checksum for the header record.
        char checkSum[8];
        /// The type of file (e.g. regular, directory, link, etc.).
        char type;
        /// Unused space (may contain data in modern TAR versions).
        uint8_t unused[355];

        /// Parse the size field and return it as a number.
        size_t parseSize() const {
            return parseNumber(size, sizeof(size));
        }

        /// Parse the checksum field and return it as a number.
        size_t parseChecksum() const {
            return parseNumber(checkSum, sizeof(checkSum));
        }

        /// Calculate the checksum of this header.
        size_t calculateChecksum() const {
            size_t sum = 0;
            const auto *data = reinterpret_cast<const uint8_t*>(this);

            for (size_t i = 0; i < sizeof(Header); i++) {
                if (i >= 148 && i < 156) { // Checksum field is treated as if it was filled with spaces
                    sum += ' ';
                } else {
                    sum += data[i];
                }
            }

            return sum;
        }

        /// Get a pointer to the file's data.
        const uint8_t* getFile() const {
            return reinterpret_cast<const uint8_t*>(this) + sizeof(Header);
        }
    } __attribute__((packed));

    /// Create a tar archive instance from a buffer containing the tar archive data.
    /// The tar archive instance will not take ownership of the memory and will not free it on destruction.
    explicit TarArchive(uint8_t *buffer);

    /// Create a tar archive instance from a File object.
    /// The file must contain a valid uncompressed tar archive.
    /// The whole file is read into memory for parsing.
    explicit TarArchive(const File &file);

    /// TarArchive is not copyable, since it manages a memory buffer.
    TarArchive(const TarArchive &other) = delete;

    /// TarArchive is not copyable, since it manages a memory buffer.
    TarArchive& operator=(const TarArchive &other) = delete;

    /// Destroy the tar archive instance and free any allocated memory.
    ~TarArchive();

    /// Get an array of all file headers in the archive.
    /// The array contains pointers to the headers within the archive buffer.
    const Array<const Header*>& getFileHeaders() const {
        return headers;
    }

    /// Get a specific file header by its path inside the archive.
    /// Returns nullptr if no such file exists.
    const Header* getHeader(const String &path) const;

private:

    enum FileType : char {
        NORMAL = 0,
        LINK = 1,
        SYMLINK = 2,
        CHR = 3,
        BLK = 4,
        DIR = 5,
        FIFO = 6,
        CONTIG = 7,
        OLDNORMAL = 48
    };

    void parseArchive();

    static size_t parseNumber(const char *string, size_t length);

    bool deleteArchiveBuffer;
    uint8_t *archiveBuffer;
    Array<const Header*> headers;
};

}
}

#endif
