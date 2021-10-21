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

File::File(const Memory::String &path) : path(path) {}

bool File::exists() const {
    auto fileDescriptor = openFile(path);
    auto ret = fileDescriptor >= 0;

    closeFile(fileDescriptor);
    return ret;
}

bool File::isFile() const {
    auto fileDescriptor = openFile(path);
    if (fileDescriptor < 0) {
        Util::Exception::throwException(Exception::INVALID_ARGUMENT, "File: Could not open file!");
    }

    auto ret = getFileType(fileDescriptor) != DIRECTORY;
    closeFile(fileDescriptor);

    return ret;
}

bool File::isDirectory() const {
    auto fileDescriptor = openFile(path);
    if (fileDescriptor < 0) {
        Util::Exception::throwException(Exception::INVALID_ARGUMENT, "File: Could not open file!");
    }

    auto ret = getFileType(fileDescriptor) == DIRECTORY;
    closeFile(fileDescriptor);

    return ret;
}

uint32_t File::File::getLength() const {
    auto fileDescriptor = openFile(path);
    if (fileDescriptor < 0) {
        Util::Exception::throwException(Exception::INVALID_ARGUMENT, "File: Could not open file!");
    }

    auto ret = getFileLength(fileDescriptor);
    closeFile(fileDescriptor);

    return ret;
}

Memory::String File::getName() const {
    const auto splitPath = ::getCanonicalPath(path).split("/");
    return splitPath.length() == 0 ? "" : splitPath[splitPath.length() - 1];
}

Memory::String File::getCanonicalPath() const {
    return ::getCanonicalPath(path);
}

Memory::String File::getParent() const {
    return ::getCanonicalPath(path + "/..");
}

Data::Array<Memory::String> File::getChildren() const {
    auto fileDescriptor = openFile(path);
    if (fileDescriptor < 0) {
        Util::Exception::throwException(Exception::INVALID_ARGUMENT, "File: Could not open file!");
    }

    auto ret = getFileChildren(fileDescriptor);
    closeFile(fileDescriptor);

    return ret;
}

File File::getParentFile() const {
    return File(getParent());
}

bool File::create(Type fileType) const {
    return createFile(path, fileType);
}

bool File::remove() const {
    return deleteFile(path);
}

}