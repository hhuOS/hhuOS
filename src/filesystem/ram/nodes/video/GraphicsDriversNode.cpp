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

#include "GraphicsDriversNode.h"

GraphicsDriversNode::GraphicsDriversNode(GraphicsNode::GraphicsMode mode) :
        GraphicsNode("drivers", mode) {

}

void GraphicsDriversNode::writeValuesToCache() {
    switch(mode) {
        case TEXT:
            if(graphicsService->getAvailableTextDrivers().length() == 0) {
                cache = "\n";
            } else {
                cache = "";

                for (const String &driver : graphicsService->getAvailableTextDrivers()) {
                    cache += driver + "\n";
                }
            }
            break;
        case LINEAR_FRAME_BUFFER:
            if(graphicsService->getAvailableLinearFrameBuffers().length() == 0) {
                cache = "\n";
            } else {
                cache = "";

                for (const String &driver : graphicsService->getAvailableLinearFrameBuffers()) {
                    cache += driver + "\n";
                }
            }
            break;
        default:
            break;
    }
}
