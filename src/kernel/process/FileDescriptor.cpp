/*
 * Copyright (C) 2017-2026 Heinrich Heine University Düsseldorf,
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

#include "FileDescriptor.h"

#include "filesystem/Node.h"
#include "lib/util/base/Panic.h"

namespace Kernel {

FileDescriptor::FileDescriptor(Filesystem::Node *node, Util::Io::File::AccessMode accessMode) : node(node), accessMode(accessMode) {}

FileDescriptor::~FileDescriptor() {
    delete node;
}

bool FileDescriptor::control(uint32_t request, const Util::Array<uint32_t> &parameters) {
    switch (request) {
        case Util::Io::File::SET_ACCESS_MODE:
            setAccessMode(static_cast<Util::Io::File::AccessMode>(parameters[0]));
            return true;
        case Util::Io::File::IS_READY_TO_READ: {
            bool &readyToRead = *reinterpret_cast<bool*>(parameters[0]);
            readyToRead = node->isReadyToRead();
            return true;
        }
        default:
            return false;
    }
}

bool FileDescriptor::isValid() const {
    return node != nullptr;
}

Filesystem::Node& FileDescriptor::getNode() const {
    if (!isValid()) {
        Util::Panic::fire(Util::Panic::INVALID_ARGUMENT, "Trying to access an invalid file descriptor!");
    }

    return *node;
}

Util::Io::File::AccessMode FileDescriptor::getAccessMode() const {
    return accessMode;
}

void FileDescriptor::setNode(Filesystem::Node *node) {
    delete FileDescriptor::node;
    FileDescriptor::node = node;
}

void FileDescriptor::setAccessMode(Util::Io::File::AccessMode accessMode) {
    FileDescriptor::accessMode = accessMode;
}

void FileDescriptor::clear() {
    delete node;
    node = nullptr;
    accessMode = Util::Io::File::BLOCKING;
}

}