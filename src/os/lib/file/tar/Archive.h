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

#ifndef __Tar_Archive_include__
#define __Tar_Archive_include__

#include "lib/file/File.h"
#include "lib/Address.h"
#include "lib/util/ArrayList.h"
#include "lib/file/tar/Constants.h"

namespace Tar {

    /**
     * @author Filip Krakowski
     */
    class Archive {

    public:

        ~Archive() = default;

        Archive(const Archive &other) = delete;

        Archive& operator=(const Archive &other) = delete;

        /**
         * Reads a tar archive from a specific file.
         *
         * @param file The file pointing to the tar archive
         * @return The tar archive
         */
        static Archive& from(File &file);

        /**
         * Reads a tar archive from a specific address in memory.
         *
         * @param address The address at which the tar archive starts
         * @return The tar archive
         */
        static Archive& from(Address &address);

    private:

        Archive() = default;

        uint32_t fileCount = 0;

        uint32_t totalSize = 0;

        Util::ArrayList<Header> headers;

        /**
         * Converts the size (base8) to the decimal system.
         *
         * @param size The size in base8
         * @return The size in base10
         */
        static uint32_t calculateSize(const uint8_t* size);

        static const uint32_t BLOCKSIZE = 0x200;
    };
}


#endif
