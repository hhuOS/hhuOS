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

#ifndef __Tar_Constants_include__
#define __Tar_Constants_include__

#include <cstdint>

namespace Tar {

    struct Header {
        uint8_t filename[100];
        uint8_t mode[8];
        uint8_t userId[8];
        uint8_t groupId[8];
        uint8_t size[12];
        uint8_t modificationTime[12];
        uint8_t checkSum[8];
        uint8_t typeFlag;
        uint8_t padding[355];

        bool operator!=(const Header &other) const;
    };

    const uint8_t LF_NORMAL    = 0;
    const uint8_t LF_LINK      = 1;
    const uint8_t LF_SYMLINK   = 2;
    const uint8_t LF_CHR       = 3;
    const uint8_t LF_BLK       = 4;
    const uint8_t LF_DIR       = 5;
    const uint8_t LF_FIFO      = 6;
    const uint8_t LF_CONTIG    = 7;
    const uint8_t LF_OLDNORMAL = 48;


}

#endif
