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

#include "RamFsNode.h"

RamFsNode::RamFsNode(VirtualNode *node) : node(node) {

}

String RamFsNode::getName() {
    return node->getName();
}

uint8_t RamFsNode::getFileType() {
    return node->getFileType();
}

uint64_t RamFsNode::getLength() {
    return node->getLength();
}

Util::Array<String> RamFsNode::getChildren() {
    Util::ArrayList<VirtualNode*> &children = node->getChildren();
    Util::Array<String> ret(children.size());

    for(uint32_t i = 0; i < children.size(); i++) {
        ret[i] = children.get(i)->getName();
    }

    return ret;
}

uint64_t RamFsNode::readData(char *buf, uint64_t pos, uint64_t numBytes) {
    uint64_t readBytes = node->readData(buf, pos, numBytes);

    if(readBytes < numBytes) {
        buf[readBytes] = END_OF_FILE;
    }

    return readBytes;
}

uint64_t RamFsNode::writeData(char *buf, uint64_t pos, uint64_t numBytes) {
    return node->writeData(buf, pos, numBytes);
}