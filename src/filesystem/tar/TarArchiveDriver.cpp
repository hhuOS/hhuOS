/*
 * Copyright (C) 2018 Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schoettner
 * Heinrich-Heine University
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

#include "TarArchiveDriver.h"
#include "TarArchiveNode.h"

TarArchiveDriver::TarArchiveDriver(Tar::Archive *archive) :
        archive(archive), fileHeaders(archive->getFileHeaders()) {

}

TarArchiveDriver::~TarArchiveDriver() {
    delete archive;
}

String TarArchiveDriver::getTypeName() {
    return TYPE_NAME;
}

bool TarArchiveDriver::createFs(StorageDevice *device) {
    return false;
}

bool TarArchiveDriver::mount(StorageDevice *device) {
    return false;
}

Util::SmartPointer<FsNode> TarArchiveDriver::getNode(const String &path) {
    for(Tar::Header *header : archive->getFileHeaders()) {
        String currentPath = header->filename;

        if(path == currentPath) {
            return Util::SmartPointer<FsNode>(new TarArchiveNode(archive, header));
        }

        if((path == "") || (currentPath.beginsWith(path) && currentPath[path.length()] == '/')) {
            return Util::SmartPointer<FsNode>(new TarArchiveNode(archive, path));
        }
    }

    return Util::SmartPointer<FsNode>(nullptr);
}

bool TarArchiveDriver::createNode(const String &path, uint8_t fileType) {
    return false;
}

bool TarArchiveDriver::deleteNode(const String &path) {
    return false;
}