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

#include "MemoryNode.h"

uint64_t MemoryNode::getLength() {
    getValues();
    return cache.length();
}

uint64_t MemoryNode::readData(char *buf, uint64_t pos, uint64_t numBytes) {
    getValues();

    uint32_t length = cache.length();

    if (pos + numBytes > length) {
        numBytes = (uint64_t) (length - pos);
    }

    memcpy(buf, (char*) cache + pos, numBytes);

    return numBytes;
}

uint64_t MemoryNode::writeData(char *buf, uint64_t pos, uint64_t numBytes) {
    return 0;
}

MemoryNode::MemoryNode(String name, FsNode::FileType type) : VirtualNode(name, type) {}
