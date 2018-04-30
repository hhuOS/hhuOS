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

#include <kernel/Kernel.h>
#include <devices/graphics/text/TextDriver.h>
#include "GraphicsMemoryNode.h"

GraphicsMemoryNode::GraphicsMemoryNode(uint8_t mode) : VirtualNode("memory", FsNode::REGULAR_FILE), mode(mode) {
    graphicsService = Kernel::getService<GraphicsService>();
}

uint64_t GraphicsMemoryNode::getLength() {
    switch(mode) {
        case TEXT :
            return String::valueOf(graphicsService->getTextDriver()->getVideoMemorySize(), 10).length() + 1;
        case LINEAR_FRAME_BUFFER :
            return String::valueOf(graphicsService->getLinearFrameBuffer()->getVideoMemorySize(), 10).length() + 1;
        default:
            return 0;
    }
}

uint64_t GraphicsMemoryNode::readData(char *buf, uint64_t pos, uint64_t numBytes) {
    String string;

    switch(mode) {
        case TEXT :
            string = String::valueOf(graphicsService->getTextDriver()->getVideoMemorySize(), 10) + "\n";
            break;
        case LINEAR_FRAME_BUFFER :
            string = String::valueOf(graphicsService->getLinearFrameBuffer()->getVideoMemorySize(), 10) + "\n";
            break;
        default:
            break;
    }

    uint64_t length = string.length();

    if (pos + numBytes > length) {
        numBytes = (uint32_t) (length - pos);
    }

    memcpy(buf, (char*) string + pos, numBytes);

    return numBytes;
}

uint64_t GraphicsMemoryNode::writeData(char *buf, uint64_t pos, uint64_t numBytes) {
    return 0;
}