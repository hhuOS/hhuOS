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

#include "VirtualNode.h"

VirtualNode::VirtualNode(const String &name, uint8_t fileType) : name(name), fileType(fileType), length(0) {

}

VirtualNode::~VirtualNode() {
    for(const auto &elemement : children) {
        delete elemement;
    }

    if(data != nullptr) {
        delete data;
    }
}

String VirtualNode::getName() {
    return name;
}

uint8_t VirtualNode::getFileType() {
    return fileType;
}

uint64_t VirtualNode::getLength() {
    return length;
}

Util::ArrayList<VirtualNode*> &VirtualNode::getChildren() {
    return children;
}

uint64_t VirtualNode::readData(char *buf, uint64_t pos, uint64_t numBytes) {
    if(pos >= length) {
        return 0;
    }

    if (pos + numBytes > length) {
        numBytes = (length - pos);
    }

    for(uint64_t i = 0; i < numBytes; i++) {
        buf[i] = data[pos + i];
    }
    
    return numBytes;
}

uint64_t VirtualNode::writeData(char *buf, uint64_t pos, uint64_t numBytes) {
    if(pos + numBytes >= length) {
        auto *newData = new char[pos + numBytes];

        memset(newData, 0, pos + numBytes);
        memcpy(newData, data, length);

        delete data;
        data = newData;

        length = pos + numBytes;
    }

    for(uint32_t i = 0; i < numBytes; i++) {
        data[i + pos] = buf[i];
    }
    
    return numBytes;
}