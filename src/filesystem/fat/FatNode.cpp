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

#include "FatNode.h"

#include "FatDirectory.h"
#include "FatFile.h"

namespace Filesystem::Fat {

FatNode::FatNode(FILINFO *info) : info(*info) {}

FatNode::~FatNode() {
    delete &info;
}

FatNode *FatNode::open(const Util::Memory::String &path) {
    // Try to stat the file. If this fails, the file is either non-existent,
    // or it may be the root-directory (f_stat will fail, when executed on the root-directory).
    auto *info = new FILINFO();
    auto result = f_stat(static_cast<const char*>(path), info);
    if (result != FR_OK) {
        if (path.endsWith(":")) {
            info->fname[0] = 0;
            info->altname[0] = 0;
            info->fattrib = AM_DIR;
        } else {
            return nullptr;
        }
    }

    if ((info->fattrib & AM_DIR) != 0) {
        DIR *directory = new DIR();
        result = f_opendir(directory, static_cast<const char*>(path));

        if (result == FR_OK) {
            return new FatDirectory(directory, info);
        }
    } else {
        FIL *file = new FIL();
        result = f_open(file, static_cast<const char*>(path), FA_READ | FA_WRITE);

        if (result == FR_OK) {
            return new FatFile(file, info);
        }
    }

    return nullptr;
}

Util::Memory::String FatNode::getName() {
    return info.fname;
}

uint64_t FatNode::getLength() {
    return info.fsize;
}

}