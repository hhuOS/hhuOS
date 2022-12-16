/*
 * Copyright (C) 2018 Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schoettner
 * Heinrich-Heine University
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

#ifndef __Tar_Archive_include__
#define __Tar_Archive_include__

#include <cstdint>

#include "lib/util/data/Array.h"
#include "lib/util/memory/String.h"
#include "lib/util/data/ArrayList.h"
#include "lib/util/data/Collection.h"
#include "lib/util/data/Iterator.h"

namespace Util::File::Tar {

class Archive {

public:

    struct Header {
        char filename[100];
        uint8_t mode[8];
        uint8_t userId[8];
        uint8_t groupId[8];
        uint8_t size[12];
        uint8_t modificationTime[12];
        uint8_t checkSum[8];
        uint8_t typeFlag;
        uint8_t padding[355];
    };
    
public:

    explicit Archive(uint32_t address);

    ~Archive() = default;

    Archive(const Archive &other) = delete;

    Archive& operator=(const Archive &other) = delete;

    /**
     * Returns all file headers within this archive.
     *
     * @return All file headers.
     */
    Util::Data::Array<Header> getFileHeaders();

    /**
     * Returns the specified file within this archive.
     *
     * @param path The file's path.
     * @return The specified file or nullptr if it does not exist.
     */
    uint8_t* getFile(const Util::Memory::String &path);

    /**
     * Converts the size (base8) to the decimal system.
     *
     * @param size The size in base8
     * @return The size in base10
     */
    static uint32_t calculateFileSize(const Header &header);

private:

    uint32_t fileCount = 0;
    uint32_t totalSize = 0;

    Util::Data::ArrayList<Header*> headers;

    static const constexpr uint8_t LF_NORMAL = 0;
    static const constexpr uint8_t LF_LINK = 1;
    static const constexpr uint8_t LF_SYMLINK = 2;
    static const constexpr uint8_t LF_CHR = 3;
    static const constexpr uint8_t LF_BLK = 4;
    static const constexpr uint8_t LF_DIR = 5;
    static const constexpr uint8_t LF_FIFO = 6;
    static const constexpr uint8_t LF_CONTIG = 7;
    static const constexpr uint8_t LF_OLDNORMAL = 48;
    static const constexpr uint32_t BLOCKSIZE = 0x200;
};

}


#endif
