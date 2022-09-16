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
    fileDescriptor = -1;
}

File& File::operator=(const File &other) {
    if (&other == this) {
        return *this;
    }

    path = other.path;
    fileDescriptor = -1;
    return *this;
}

File::~File() {
    if (fileDescriptor != -1) {
        ::closeFile(fileDescriptor);
    }
}

bool File::exists() {
    ensureFileIsOpened();
    if (fileDescriptor >= 0) {
        return true;
    }

    return false;
}

Type File::getType() {
    ensureFileIsOpened();
    if (fileDescriptor < 0) {
        Util::Exception::throwException(Exception::INVALID_ARGUMENT, "File: Could not open file!");
    }

    return ::getFileType(fileDescriptor);
}

bool File::isFile() {
    return getType() != DIRECTORY;
}

bool File::isDirectory() {
    return getType() == DIRECTORY;
}

uint32_t File::File::getLength() {
    ensureFileIsOpened();
    if (fileDescriptor < 0) {
        Util::Exception::throwException(Exception::INVALID_ARGUMENT, "File: Could not open file!");
    }

    return ::getFileLength(fileDescriptor);
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

Data::Array<Memory::String> File::getChildren() {
    ensureFileIsOpened();
    if (fileDescriptor < 0) {
        Util::Exception::throwException(Exception::INVALID_ARGUMENT, "File: Could not open file!");
    }

    return ::getFileChildren(fileDescriptor);
}

File File::getParentFile() const {
    return File(getParent());
}

bool File::create(Type fileType) {
    auto ret = ::createFile(path, fileType);
    fileDescriptor = -1;

    return ret;
}

bool File::remove() {
    auto ret = ::deleteFile(path);
    fileDescriptor = -1;

    return ret;
}

bool File::control(uint32_t request, const Data::Array<uint32_t> &parameters) {
    ensureFileIsOpened();
    if (fileDescriptor < 0) {
        Util::Exception::throwException(Exception::INVALID_ARGUMENT, "File: Could not open file!");
    }

    return ::controlFile(fileDescriptor, request, parameters);
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
                parsedToken.removeIndex(parsedToken.size() - 1);
            }
        } else {
            parsedToken.add(string);
        }
    }

    if (parsedToken.isEmpty()) {
        return "";
    }

    Util::Memory::String parsedPath = Util::File::File::SEPARATOR + Util::Memory::String::join(Util::File::File::SEPARATOR, parsedToken.toArray());
    return parsedPath;
}

void File::ensureFileIsOpened() {
    if (fileDescriptor < 0) {
        fileDescriptor = ::openFile(path);
    }
}

bool mount(const Util::Memory::String &device, const Util::Memory::String &targetPath, const Util::Memory::String &driverName) {
    return ::mount(device, targetPath, driverName);
}

bool unmount(const Util::Memory::String &path) {
    return ::unmount(path);
}

int32_t open(const Memory::String &path) {
    return ::openFile(path);
}

bool controlFile(int32_t fileDescriptor, uint32_t request, const Util::Data::Array<uint32_t> &parameters) {
    return ::controlFile(fileDescriptor, request, parameters);
}

void close(int32_t fileDescriptor) {
    return ::closeFile(fileDescriptor);
}

bool changeDirectory(const Util::Memory::String &path) {
    return ::changeDirectory(path);
}

File getCurrentWorkingDirectory() {
    return ::getCurrentWorkingDirectory();
}

const char* getFileColor(File &file) {
    switch (file.getType()) {
        case DIRECTORY:
            return Util::Graphic::Ansi::FOREGROUND_BRIGHT_BLUE;
        case REGULAR:
            return Util::Graphic::Ansi::FOREGROUND_WHITE;
        case CHARACTER:
            return Util::Graphic::Ansi::FOREGROUND_BRIGHT_YELLOW;
    }

    return Util::Graphic::Ansi::FOREGROUND_WHITE;
}

}