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

#include "Directory.h"

Directory::Directory(Util::SmartPointer<FsNode> node, const String &path) : node(node) {
    this->path = FileSystem::parsePath(path);
};

Directory *Directory::open(const String &path) {
    auto *fileSystem = Kernel::getService<FileSystem>();

    Util::SmartPointer<FsNode> node = fileSystem->getNode(path);

    if(node != nullptr && node->getFileType() == FsNode::DIRECTORY_FILE) {
        return new Directory(node, path);
    }

    return nullptr;
}

String Directory::getName() {
    if(node->getName() == "/" && path != "/") {
        // This directory is the root of a mount-point.
        // We need to find the name, of the directory, where it is mounted in.
        Util::Array<String> tmp =  String(path).split(path, '/');

        if(tmp.length() == 0) {
            return path.substring(1, path.length());
        }

        return tmp[tmp.length() - 1];
    }

    return node->getName();
}

String Directory::getAbsolutePath() {
    return path;
}

Util::Array<String> Directory::getChildren() {
    return node->getChildren();
}