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

#include <cstdint>

#include "lib/util/file/File.h"
#include "MemoryDriver.h"
#include "MemoryDirectoryNode.h"
#include "MemoryFileNode.h"
#include "MemoryWrapperNode.h"
#include "filesystem/memory/MemoryNode.h"
#include "lib/util/Exception.h"
#include "lib/util/data/Array.h"
#include "lib/util/data/ArrayList.h"

namespace Filesystem {
class Node;
}  // namespace Filesystem

namespace Filesystem::Memory {

MemoryDriver::MemoryDriver() : rootNode(new MemoryDirectoryNode("")) {}

MemoryDriver::~MemoryDriver() {
    delete rootNode;
}

Node *MemoryDriver::getNode(const Util::Memory::String &path) {
    if(path.length() == 0){
        return new MemoryWrapperNode(*rootNode);
    }

    Util::Data::Array<Util::Memory::String> tokens = path.split(Util::File::File::SEPARATOR);
    if(tokens.length() == 0) {
        return new MemoryWrapperNode(*rootNode);
    }

    MemoryNode *currentDir = rootNode;
    for(uint32_t i = 0; i < tokens.length() - 1; i++) {
        currentDir = reinterpret_cast<MemoryDirectoryNode*>(currentDir)->getChildByName(tokens[i]);
        if(currentDir == nullptr || currentDir->getFileType() != Util::File::DIRECTORY) {
            return nullptr;
        }
    }

    MemoryNode *ret = reinterpret_cast<MemoryDirectoryNode*>(currentDir)->getChildByName(tokens[tokens.length() - 1]);
    if(ret == nullptr) {
        return nullptr;
    }

    return new MemoryWrapperNode(*ret);
}

bool MemoryDriver::createNode(const Util::Memory::String &path, Util::File::Type type) {
    if (type != Util::File::DIRECTORY && type != Util::File::REGULAR) {
        Util::Exception::throwException(Util::Exception::INVALID_ARGUMENT, "MemoryDriver: Invalid file type!");
    }

    Util::Data::Array<Util::Memory::String> tokens = path.split(Util::File::File::SEPARATOR);
    if(path.length() > 0 && tokens.length() == 0) {
        return false;
    }

    MemoryNode *currentDir = rootNode;
    for(uint32_t i = 0; i < tokens.length() - 1; i++) {
        currentDir = reinterpret_cast<MemoryDirectoryNode*>(currentDir)->getChildByName(tokens[i]);
        if(currentDir == nullptr || currentDir->getFileType() != Util::File::DIRECTORY) {
            return false;
        }
    }

    if(reinterpret_cast<MemoryDirectoryNode*>(currentDir)->getChildByName(tokens[tokens.length() - 1]) != nullptr) {
        return false;
    }

    const auto &name = tokens[tokens.length() - 1];
    auto *newNode = type == Util::File::DIRECTORY ? reinterpret_cast<MemoryNode*>(new MemoryDirectoryNode(name)) : reinterpret_cast<MemoryNode*>(new MemoryFileNode(name));
    reinterpret_cast<MemoryDirectoryNode*>(currentDir)->children.add(newNode);

    return true;
}

bool MemoryDriver::deleteNode(const Util::Memory::String &path) {
    Util::Data::Array<Util::Memory::String> tokens = path.split(Util::File::File::SEPARATOR);
    if(path.length() > 0 && tokens.length() == 0) {
        return false;
    }

    MemoryNode *currentDir = rootNode;
    for(uint32_t i = 0; i < tokens.length() - 1; i++) {
        currentDir = reinterpret_cast<MemoryDirectoryNode*>(currentDir)->getChildByName(tokens[i]);
        if(currentDir == nullptr || currentDir->getFileType() != Util::File::DIRECTORY) {
            return false;
        }
    }

    MemoryNode *node = reinterpret_cast<MemoryDirectoryNode*>(currentDir)->getChildByName(tokens[tokens.length() - 1]);
    if(node == nullptr) {
        return false;
    }

    if(node->getChildren().length() > 0) {
        return false;
    }

    reinterpret_cast<MemoryDirectoryNode*>(currentDir)->children.remove(node);
    delete node;

    return true;
}

bool MemoryDriver::addNode(const Util::Memory::String &path, MemoryNode *node) {
    Util::Data::Array<Util::Memory::String> tokens = path.split(Util::File::File::SEPARATOR);

    MemoryNode *currentDir = rootNode;
    for (const auto &token : tokens) {
        currentDir = reinterpret_cast<MemoryDirectoryNode*>(currentDir)->getChildByName(token);
        if (currentDir == nullptr || currentDir->getFileType() != Util::File::DIRECTORY) {
            return false;
        }
    }

    auto *checkNode = reinterpret_cast<MemoryDirectoryNode*>(currentDir)->getChildByName(node->getName());
    if (checkNode != nullptr) {
        return false;
    }

    reinterpret_cast<MemoryDirectoryNode*>(currentDir)->addChild(node);
    return true;
}

}