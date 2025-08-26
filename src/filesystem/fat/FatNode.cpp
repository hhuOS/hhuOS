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

#include "FatNode.h"

#include "FatDirectory.h"
#include "FatFile.h"

namespace Filesystem::Fat {

FatNode::FatNode(const Util::String &path) : path(path) {}

FatNode *FatNode::open(const Util::String &path) {
    // Try to stat the file. If this fails, the file is either non-existent,
    // or it may be the root-directory (f_stat will fail, when executed on the root-directory).
    FILINFO info{};
    auto result = f_stat(static_cast<const char*>(path), &info);
    if (result != FR_OK) {
        if (path.endsWith(":")) {
            info.fname[0] = 0;
            info.altname[0] = 0;
            info.fattrib = AM_DIR;
        } else {
            return nullptr;
        }
    }

    if ((info.fattrib & AM_DIR) != 0) {
        DIR directory{};
        result = f_opendir(&directory, static_cast<const char*>(path));

        if (result == FR_OK) {
            return new FatDirectory(directory, path);
        }
    } else {
        FIL file{};
        result = f_open(&file, static_cast<const char*>(path), FA_READ | FA_WRITE);

        if (result == FR_OK) {
            return new FatFile(file, path);
        }
    }

    return nullptr;
}

Util::String FatNode::getName() {
    const auto info = stat();
    return info.fname;
}

uint64_t FatNode::getLength() {
    const auto info = stat();
    return info.fsize;
}

FILINFO FatNode::stat() {
    FILINFO info{};
    auto result = f_stat(static_cast<const char*>(path), &info);
    if (result != FR_OK) {
        if (path.endsWith(":")) {
            info.fname[0] = 0;
            info.altname[0] = 0;
            info.fattrib = AM_DIR;
        } else {
            Util::Panic::fire(Util::Panic::ILLEGAL_STATE, "FatNode: Could not update file info!");
        }
    }

    return info;
}

}
