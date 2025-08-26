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

#include "File.h"

#include "lib/interface.h"
#include "collection/ArrayList.h"
#include "base/Panic.h"

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
        closeFile(fileDescriptor);
    }
}

bool File::exists() const {
    const auto tempDescriptor = openFile(path);

    if (tempDescriptor > 0) {
        closeFile(tempDescriptor);
        return true;
    }

    return false;
}

File::Type File::getType() const {
    ensureFileIsOpened();
    return getFileType(fileDescriptor);
}

bool File::isFile() const {
    return getType() != DIRECTORY;
}

bool File::isDirectory() const {
    return getType() == DIRECTORY;
}

size_t File::getLength() const {
    ensureFileIsOpened();
    return getFileLength(fileDescriptor);
}

String File::getName() const {
    const auto splitPath = getCanonicalPath(path).split("/");
    return splitPath.length() == 0 ? "" : splitPath[splitPath.length() - 1];
}

String File::getCanonicalPath() const {
    return getCanonicalPath(path);
}

File File::getParent() const {
    return File(getCanonicalPath(path + "/.."));
}

Array<File> File::getChildren() const {
    ensureFileIsOpened();

    const auto childrenNames = getFileChildren(fileDescriptor);
    auto children = Array<File>(childrenNames.length());

    for (uint32_t i = 0; i < childrenNames.length(); i++) {
        children[i] = File(getCanonicalPath() + '/' + childrenNames[i]);
    }

    return children;
}

bool File::create(const Type fileType) {
    if (fileDescriptor >= 0) {
        closeFile(fileDescriptor);
        fileDescriptor = -1;
    }

    return createFile(path, fileType);
}

bool File::remove() {
    if (fileDescriptor >= 0) {
        closeFile(fileDescriptor);
        fileDescriptor = -1;
    }

    return deleteFile(path);
}

bool File::controlFile(const size_t request, const Array<size_t> &parameters) {
    ensureFileIsOpened();
    return ::controlFile(fileDescriptor, request, parameters);
}

String File::getCanonicalPath(const String &path) {
    if (path.isEmpty()) {
        return "";
    }

    const auto absolutePath = path[0] == '/' ?
        path : getCurrentWorkingDirectory().getCanonicalPath() + '/' + path;
    const Array<String> tokens = absolutePath.split('/');
    ArrayList<String> parsedToken;

    for (const String &string : tokens) {
        if (string == ".") {
            continue;
        }
        if (string == "..") {
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

    String parsedPath = '/' + String::join('/', parsedToken.toArray());
    return parsedPath;
}

void File::ensureFileIsOpened() const {
    if (fileDescriptor < 0) {
        fileDescriptor = openFile(path);
    }

    if (fileDescriptor < 0) {
        Util::Panic::fire(Panic::INVALID_ARGUMENT, "File: Could not open file!");
    }
}

bool File::mount(const String &device, const String &targetPath, const String &driverName) {
    return ::mount(device, targetPath, driverName);
}

bool File::unmount(const String &path) {
    return ::unmount(path);
}

int32_t File::open(const String &path) {
    return openFile(path);
}

bool File::controlFile(const int32_t fileDescriptor, const size_t request, const Array<size_t> &parameters) {
    return ::controlFile(fileDescriptor, request, parameters);
}

bool File::controlFileDescriptor(const int32_t fileDescriptor, const FileDescriptorRequest request,
        const Array<size_t> &parameters)
{
    return ::controlFileDescriptor(fileDescriptor, request, parameters);
}

bool File::setAccessMode(const int32_t fileDescriptor, AccessMode accessMode) {
    return controlFileDescriptor(fileDescriptor, SET_ACCESS_MODE, Util::Array<size_t>({accessMode}));
}

bool File::isReadyToRead(const int32_t fileDescriptor) {
    bool readyToRead;
    const auto success = controlFileDescriptor(fileDescriptor, IS_READY_TO_READ,
        Util::Array<size_t>({reinterpret_cast<size_t>(&readyToRead)}));

    if (!success) {
        Util::Panic::fire(Panic::INVALID_ARGUMENT, "File: Failed to query readiness!");
    }

    return readyToRead;
}

void File::close(const int32_t fileDescriptor) {
    return closeFile(fileDescriptor);
}

bool File::changeDirectory(const String &path) {
    return ::changeDirectory(path);
}

File File::getCurrentWorkingDirectory() {
    return ::getCurrentWorkingDirectory();
}

}