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

#include "lib/interface.h"
#include "lib/util/data/ArrayList.h"
#include "lib/util/graphic/Ansi.h"
#include "File.h"

namespace Util::File {

File::File(const Memory::String &path) : path(path) {}

File::File(const File &copy) {
    path = copy.path;
}

File &File::operator=(const File &other) {
    if (&other == this) {
        return *this;
    }

    path = other.path;
    return *this;
}

bool File::exists() const {
    auto fileDescriptor = open(path);
    if (fileDescriptor >= 0) {
        close(fileDescriptor);
        return true;
    }

    return false;
}

Type File::getType() const {
    auto fileDescriptor = open(path);
    if (fileDescriptor < 0) {
        Util::Exception::throwException(Exception::INVALID_ARGUMENT, "File: Could not open file!");
    }

    auto ret = getFileType(fileDescriptor);
    close(fileDescriptor);

    return ret;
}

bool File::isFile() const {
    return getType() != DIRECTORY;
}

bool File::isDirectory() const {
    return getType() == DIRECTORY;
}

uint32_t File::File::getLength() const {
    auto fileDescriptor = open(path);
    if (fileDescriptor < 0) {
        Util::Exception::throwException(Exception::INVALID_ARGUMENT, "File: Could not open file!");
    }

    auto ret = getFileLength(fileDescriptor);
    close(fileDescriptor);

    return ret;
}

Memory::String File::getName() const {
    const auto splitPath = getCanonicalPath(path).split("/");
    return splitPath.length() == 0 ? "" : splitPath[splitPath.length() - 1];
}

Memory::String File::getCanonicalPath() const {
    return getCanonicalPath(path);
}

Memory::String File::getParent() const {
    return getCanonicalPath(path + "/..");
}

Data::Array<Memory::String> File::getChildren() const {
    auto fileDescriptor = open(path);
    if (fileDescriptor < 0) {
        Util::Exception::throwException(Exception::INVALID_ARGUMENT, "File: Could not open file!");
    }

    auto ret = getFileChildren(fileDescriptor);
    close(fileDescriptor);

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

Util::Memory::String File::getCanonicalPath(const Util::Memory::String &path) {
    if (path.isEmpty()) {
        return "";
    }

    auto absolutePath = path[0] == '/' ? path : getCurrentWorkingDirectory().getCanonicalPath() + SEPARATOR + path;
    Util::Data::Array<Util::Memory::String> token = absolutePath.split(Util::File::File::SEPARATOR);
    Util::Data::ArrayList<Util::Memory::String> parsedToken;

    for (const Util::Memory::String &string : token) {
        if (string == ".") {
            continue;
        } else if (string == "..") {
            if (!parsedToken.isEmpty()) {
                parsedToken.remove(parsedToken.size() - 1);
            }
        } else {
            parsedToken.add(*new Util::Memory::String(string));
        }
    }

    if (parsedToken.isEmpty()) {
        return "";
    }

    Util::Memory::String parsedPath = Util::File::File::SEPARATOR + Util::Memory::String::join(Util::File::File::SEPARATOR, parsedToken.toArray());
    return parsedPath;
}

int32_t open(const Memory::String &path) {
    return openFile(path);
}

void close(int32_t fileDescriptor) {
    return closeFile(fileDescriptor);
}

bool changeDirectory(const Util::Memory::String &path) {
    return ::changeDirectory(path);
}

File getCurrentWorkingDirectory() {
    return ::getCurrentWorkingDirectory();
}

const char* getFileColor(const File &file) {
    switch (file.getType()) {
        case DIRECTORY:
            return Util::Graphic::Ansi::BRIGHT_BLUE;
        case REGULAR:
            return Util::Graphic::Ansi::WHITE;
        case CHARACTER:
            return Util::Graphic::Ansi::BRIGHT_YELLOW;
    }

    return Util::Graphic::Ansi::WHITE;
}

}