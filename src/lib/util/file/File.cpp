/*
 * Copyright (C) 2018-2021 Heinrich-Heine-Universitaet Duesseldorf,
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

#include <lib/interface.h>
#include "File.h"

namespace Util::File {

File::File(const Memory::String &path) : path(path), node(openFile(path)) {}

File::~File() {
    delete node;
}

bool File::exists() const {
    return node != nullptr;
}

bool File::isFile() const {
    return node != nullptr && (node->getFileType() == REGULAR);
}

bool File::isDirectory() const {
    return node != nullptr && (node->getFileType() == DIRECTORY);
}

uint32_t File::File::getLength() const {
    return node == nullptr ? 0 : node->getLength();
}

Memory::String File::getName() const {
    return node == nullptr ? "" : node->getName();
}

Memory::String File::getCanonicalPath() const {
    return ::getCanonicalPath(path);
}

Memory::String File::getParent() const {
    return ::getCanonicalPath(path + "/..");
}

File File::getParentFile() const {
    return File(getParent());
}

bool File::create(Type fileType) {
    bool ret = createFile(path, fileType);
    if (ret) {
        node = openFile(path);
    }

    return ret;
}

}