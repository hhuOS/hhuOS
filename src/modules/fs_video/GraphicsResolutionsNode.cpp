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

GraphicsResolutionsNode::GraphicsResolutionsNode(GraphicsNode::GraphicsMode mode) : GraphicsNode("resolutions", mode) {

}

void GraphicsResolutionsNode::writeValuesToCache() {
    switch(mode) {
        case TEXT : {
            if(graphicsService->getTextDriver() == nullptr) {
                cache = "0x0@0\n";
            } else {
                Util::Array<TextDriver::TextResolution> textResolutions = graphicsService->getTextDriver()->getTextResolutions();

                cache = "";

                for (const TextDriver::TextResolution &currentRes : textResolutions) {
                    cache += String::valueOf(currentRes.columns, 10) + String("x") +
                             String::valueOf(currentRes.rows, 10) + String("@") +
                             String::valueOf(currentRes.depth, 10) + String("\n");
                }
            }

            break;
        }
        case LINEAR_FRAME_BUFFER : {
            if(graphicsService->getLinearFrameBuffer() == nullptr) {
                cache = "0x0@0\n";
            } else {
                Util::Array<LinearFrameBuffer::LfbResolution> lfbResolutions = graphicsService->getLinearFrameBuffer()->getLfbResolutions();

                cache = "";

                for (const LinearFrameBuffer::LfbResolution &currentRes : lfbResolutions) {
                    cache += String::valueOf(currentRes.resX, 10) + String("x") +
                             String::valueOf(currentRes.resY, 10) + String("@") +
                             String::valueOf(currentRes.depth, 10) + String("\n");
                }
            }

            break;
        }
        default:
            break;
    }
}