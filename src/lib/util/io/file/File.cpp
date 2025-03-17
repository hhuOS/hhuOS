/*
 * Copyright (C) 2018-2025 Heinrich-Heine-Universitaet Duesseldorf,
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
#include "lib/util/collection/ArrayList.h"
#include "lib/util/graphic/Ansi.h"
#include "lib/util/io/file/File.h"
#include "lib/util/base/Exception.h"

namespace Util::Io {

File::File(const String &path) : path(path) {}

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
    auto tempDescriptor = ::openFile(path);

    if (tempDescriptor > 0) {
        ::closeFile(tempDescriptor);
        return true;
    }

    return false;
}

File::Type File::getType() {
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

uint32_t Io::File::getLength() {
    ensureFileIsOpened();
    if (fileDescriptor < 0) {
        Util::Exception::throwException(Exception::INVALID_ARGUMENT, "File: Could not open file!");
    }

    return ::getFileLength(fileDescriptor);
}

String File::getName() const {
    const auto splitPath = getCanonicalPath(path).split("/");
    return splitPath.length() == 0 ? "" : splitPath[splitPath.length() - 1];
}

String File::getCanonicalPath() const {
    return getCanonicalPath(path);
}

String File::getParent() const {
    return getCanonicalPath(path + "/..");
}

Array<String> File::getChildren() {
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

bool File::controlFile(uint32_t request, const Util::Array<uint32_t> &parameters) {
    ensureFileIsOpened();
    if (fileDescriptor < 0) {
        Util::Exception::throwException(Exception::INVALID_ARGUMENT, "File: Could not open file!");
    }

    return ::controlFile(fileDescriptor, request, parameters);
}

bool File::controlFileDescriptor(uint32_t request, const Array<uint32_t> &parameters) {
    ensureFileIsOpened();
    if (fileDescriptor < 0) {
        Util::Exception::throwException(Exception::INVALID_ARGUMENT, "File: Could not open file!");
    }

    return ::controlFileDescriptor(fileDescriptor, request, parameters);
}

bool File::setAccessMode(File::AccessMode accessMode) {
    ensureFileIsOpened();
    if (fileDescriptor < 0) {
        Util::Exception::throwException(Exception::INVALID_ARGUMENT, "File: Could not open file!");
    }

    return controlFileDescriptor(fileDescriptor, Util::Io::File::SET_ACCESS_MODE, Util::Array<uint32_t>({accessMode}));
}

bool File::isReadyToRead() {
    ensureFileIsOpened();
    if (fileDescriptor < 0) {
        Util::Exception::throwException(Exception::INVALID_ARGUMENT, "File: Could not open file!");
    }

    return isReadyToRead(fileDescriptor);
}

Util::String File::getCanonicalPath(const Util::String &path) {
    if (path.isEmpty()) {
        return "";
    }

    auto absolutePath = path[0] == '/' ? path : getCurrentWorkingDirectory().getCanonicalPath() + SEPARATOR + path;
    Util::Array<Util::String> token = absolutePath.split(Util::Io::File::SEPARATOR);
    Util::ArrayList<Util::String> parsedToken;

    for (const Util::String &string : token) {
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

    Util::String parsedPath = Util::Io::File::SEPARATOR + Util::String::join(Util::Io::File::SEPARATOR, parsedToken.toArray());
    return parsedPath;
}

void File::ensureFileIsOpened() {
    if (fileDescriptor < 0) {
        fileDescriptor = ::openFile(path);
    }
}

bool File::mount(const Util::String &device, const Util::String &targetPath, const Util::String &driverName) {
    return ::mount(device, targetPath, driverName);
}

bool File::unmount(const Util::String &path) {
    return ::unmount(path);
}

int32_t File::open(const String &path) {
    return ::openFile(path);
}

bool File::controlFile(int32_t fileDescriptor, uint32_t request, const Util::Array<uint32_t> &parameters) {
    return ::controlFile(fileDescriptor, request, parameters);
}

bool File::controlFileDescriptor(int32_t fileDescriptor, uint32_t request, const Array<uint32_t> &parameters) {
    return ::controlFileDescriptor(fileDescriptor, request, parameters);
}

bool File::setAccessMode(int32_t fileDescriptor, File::AccessMode accessMode) {
    return controlFileDescriptor(fileDescriptor, SET_ACCESS_MODE, Util::Array<uint32_t>({accessMode}));
}

bool File::isReadyToRead(int32_t fileDescriptor) {
    bool readyToRead;
    if (!controlFileDescriptor(fileDescriptor, Util::Io::File::IS_READY_TO_READ, Util::Array<uint32_t>({reinterpret_cast<uint32_t>(&readyToRead)}))) {
        Util::Exception::throwException(Exception::INVALID_ARGUMENT, "File: Failed to query readiness!");
    }

    return readyToRead;
}

void File::close(int32_t fileDescriptor) {
    return ::closeFile(fileDescriptor);
}

bool File::changeDirectory(const Util::String &path) {
    return ::changeDirectory(path);
}

File File::getCurrentWorkingDirectory() {
    return ::getCurrentWorkingDirectory();
}

const char* File::getTypeColor(File &file) {
    switch (file.getType()) {
        case Util::Io::File::DIRECTORY:
            return Util::Graphic::Ansi::FOREGROUND_BRIGHT_BLUE;
        case Util::Io::File::REGULAR:
            return Util::Graphic::Ansi::FOREGROUND_WHITE;
        case Util::Io::File::CHARACTER:
            return Util::Graphic::Ansi::FOREGROUND_BRIGHT_YELLOW;
    }

    return Util::Graphic::Ansi::FOREGROUND_WHITE;
}

}