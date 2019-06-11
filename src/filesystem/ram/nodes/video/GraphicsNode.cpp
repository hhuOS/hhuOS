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

#include "kernel/core/System.h"
#include "kernel/service/EventBus.h"
#include "GraphicsNode.h"

GraphicsNode::GraphicsNode(String name, GraphicsMode mode) : VirtualNode(name, FsNode::REGULAR_FILE), mode(mode) {
    graphicsService = Kernel::System::getService<Kernel::GraphicsService>();
}

uint64_t GraphicsNode::getLength() {
    writeValuesToCache();

    return cache.length();
}

uint64_t GraphicsNode::readData(char *buf, uint64_t pos, uint64_t numBytes) {
    writeValuesToCache();

    uint64_t length = cache.length();

    if (pos + numBytes > length) {
        numBytes = (uint32_t) (length - pos);
    }

    memcpy(buf, (char*) cache + pos, numBytes);

    return numBytes;
}

uint64_t GraphicsNode::writeData(char *buf, uint64_t pos, uint64_t numBytes) {
    return 0;
}
