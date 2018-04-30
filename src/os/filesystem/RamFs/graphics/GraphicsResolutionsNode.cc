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
#include "GraphicsResolutionsNode.h"

GraphicsResolutionsNode::GraphicsResolutionsNode(uint8_t mode) : VirtualNode("resolutions", FsNode::REGULAR_FILE), mode(mode) {
    graphicsService = Kernel::getService<GraphicsService>();
}

uint64_t GraphicsResolutionsNode::getLength() {
    String string;

    switch(mode) {
        case TEXT : {
            Util::Array<TextDriver::TextResolution> textResolutions = graphicsService->getTextDriver()->getTextResolutions();

            for (const TextDriver::TextResolution &currentRes : textResolutions) {
                string += String::valueOf(currentRes.columns, 10) + String(" x ") +
                          String::valueOf(currentRes.rows, 10) + String(" @ ") +
                          String::valueOf(currentRes.depth, 10) + String("\n");
            }

            break;
        }
        case LINEAR_FRAME_BUFFER : {
            Util::Array<LinearFrameBuffer::LfbResolution> lfbResolutions = graphicsService->getLinearFrameBuffer()->getLfbResolutions();

            for (const LinearFrameBuffer::LfbResolution &currentRes : lfbResolutions) {
                string += String::valueOf(currentRes.resX, 10) + String(" x ") +
                          String::valueOf(currentRes.resY, 10) + String(" @ ") +
                          String::valueOf(currentRes.depth, 10) + String("\n");
            }

            break;
        }
        default:
            break;
    }

    return string.length();
}

uint64_t GraphicsResolutionsNode::readData(char *buf, uint64_t pos, uint64_t numBytes) {
    String string;

    switch(mode) {
        case TEXT : {
            Util::Array<TextDriver::TextResolution> textResolutions = graphicsService->getTextDriver()->getTextResolutions();

            for (const TextDriver::TextResolution &currentRes : textResolutions) {
                string += String::valueOf(currentRes.columns, 10) + String(" x ") +
                          String::valueOf(currentRes.rows, 10) + String(" @ ") +
                          String::valueOf(currentRes.depth, 10) + String("\n");
            }

            break;
        }
        case LINEAR_FRAME_BUFFER : {
            Util::Array<LinearFrameBuffer::LfbResolution> lfbResolutions = graphicsService->getLinearFrameBuffer()->getLfbResolutions();

            for (const LinearFrameBuffer::LfbResolution &currentRes : lfbResolutions) {
                string += String::valueOf(currentRes.resX, 10) + String(" x ") +
                          String::valueOf(currentRes.resY, 10) + String(" @ ") +
                          String::valueOf(currentRes.depth, 10) + String("\n");
            }

            break;
        }
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

uint64_t GraphicsResolutionsNode::writeData(char *buf, uint64_t pos, uint64_t numBytes) {
    return 0;
}