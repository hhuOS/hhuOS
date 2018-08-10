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

#include <filesystem/FileSystem.h>
#include <lib/util/List.h>
#include "RamFsDriver.h"
#include "RamFsNode.h"

RamFsDriver::~RamFsDriver() {
    if(rootNode != nullptr) {
        delete rootNode;
    }
}

VirtualNode *RamFsDriver::getChildByName(VirtualNode *parent, const String &path) {
    Util::ArrayList<VirtualNode*> &children = parent->getChildren();

    for(uint32_t i = 0; i < children.size(); i++) {
        if(children.get(i)->getName() == path) {
            return children.get(i);
        }
    }
    
    return nullptr;
}

String RamFsDriver::getName() {
    return NAME;
}

bool RamFsDriver::mount(StorageDevice *disk) {
    rootNode = new VirtualNode("/", FsNode::DIRECTORY_FILE);
    
    return true;
}

bool RamFsDriver::createFs(StorageDevice *device) {
    return false;
}

FsNode *RamFsDriver::getNode(const String &path) {
    if(path.length() == 0){
        return new RamFsNode(rootNode);
    }

    Util::Array<String> token = path.split(FileSystem::SEPARATOR);

    VirtualNode *currentDir = rootNode;
    
    if(token.length() == 0) {
        return new RamFsNode(rootNode);
    }
    
    for(uint32_t i = 0; i < token.length() - 1; i++) {
        currentDir = getChildByName(currentDir, token[i]);

        if(currentDir == nullptr || currentDir->getFileType() != FsNode::DIRECTORY_FILE) {
            return nullptr;
        }
    }
    
    VirtualNode *ret = getChildByName(currentDir, token[token.length() - 1]);

    if(ret == nullptr) {
        return nullptr;
    }

    return new RamFsNode(ret);
}

bool RamFsDriver::addNode(const String &path, VirtualNode *node) {
    Util::Array<String> token = path.split(FileSystem::SEPARATOR);
    VirtualNode *currentDir = rootNode;
    
    for(uint32_t i = 0; i < token.length(); i++) {
        currentDir = getChildByName(currentDir, token[i]);

        if(currentDir == nullptr || currentDir->getFileType() != FsNode::DIRECTORY_FILE) {
            return false;
        }
    }

    if(getChildByName(currentDir, node->getName()) != nullptr) {
        return false;
    }

    currentDir->getChildren().add(node);
    return true;
}

bool RamFsDriver::createNode(const String &path, uint8_t fileType) {
    Util::Array<String> token = path.split(FileSystem::SEPARATOR);

    VirtualNode *currentDir = rootNode;

    if(path.length() > 0 && token.length() == 0) {
        return false;
    }
    
    for(uint32_t i = 0; i < token.length() - 1; i++) {
        currentDir = getChildByName(currentDir, token[i]);

        if(currentDir == nullptr || currentDir->getFileType() != FsNode::DIRECTORY_FILE) {
            return false;
        }
    }

    if(getChildByName(currentDir, token[token.length() - 1]) != nullptr) {
        return false;
    }

    auto *newNode = new VirtualNode(token[token.length() - 1], fileType);
    currentDir->getChildren().add(newNode);

    return true;
}

bool RamFsDriver::deleteNode(const String &path) {
    Util::Array<String> token = path.split(FileSystem::SEPARATOR);

    VirtualNode *currentDir = rootNode;
    
    if(path.length() > 0 && token.length() == 0) {
        return false;
    }
    
    for(uint32_t i = 0; i < token.length() - 1; i++) {
        currentDir = getChildByName(currentDir, token[i]);

        if(currentDir == nullptr || currentDir->getFileType() != FsNode::DIRECTORY_FILE) {
            return false;
        }
    }
    
    VirtualNode *node = getChildByName(currentDir, token[token.length() - 1]);

    if(node == nullptr) {
        return false;
    }

    if(!node->getChildren().isEmpty()) {
        delete node;
        return false;
    }

    currentDir->getChildren().remove(node);

    delete node;
    return true;
}
