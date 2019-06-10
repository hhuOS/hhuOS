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

#include "CurrentGraphicsDriverNode.h"

CurrentGraphicsDriverNode::CurrentGraphicsDriverNode(GraphicsNode::GraphicsMode mode) :
        GraphicsNode("driver", mode) {

}

void CurrentGraphicsDriverNode::writeValuesToCache() {
    switch(mode) {
        case TEXT:
            if(graphicsService->getTextDriver() == nullptr) {
                cache = "\n";
            } else {
                cache = graphicsService->getTextDriver()->getName().toLowerCase() + "\n";
            }
            break;
        case LINEAR_FRAME_BUFFER:
            if(graphicsService->getLinearFrameBuffer() == nullptr) {
                cache = "\n";
            } else {
                cache = graphicsService->getLinearFrameBuffer()->getName().toLowerCase() + "\n";
            }
            break;
        default:
            break;
    }
}

uint64_t CurrentGraphicsDriverNode::writeData(char *buf, uint64_t pos, uint64_t numBytes) {
    char *tmp = new char[numBytes + 1];

    memcpy(tmp, buf, numBytes);
    tmp[numBytes] = 0;

    String buffer = String(tmp).removeAll("\n").strip();

    delete[] tmp;

    switch(mode) {
        case TEXT: {
            TextDriver::TextResolution res{0};

            if (graphicsService->getTextDriver() != nullptr) {
                res.columns = graphicsService->getTextDriver()->getColumnCount();
                res.rows = graphicsService->getTextDriver()->getRowCount();
                res.depth = graphicsService->getTextDriver()->getDepth();
            }

            if(graphicsService->setTextDriver(buffer)) {
                graphicsService->getTextDriver()->init(res.columns, res.rows, res.depth);
            }

            break;
        }
        case LINEAR_FRAME_BUFFER:{
            LinearFrameBuffer::LfbResolution res{0};

            if (graphicsService->getLinearFrameBuffer() != nullptr) {
                res.resX = graphicsService->getLinearFrameBuffer()->getResX();
                res.resY = graphicsService->getLinearFrameBuffer()->getResY();
                res.depth = graphicsService->getLinearFrameBuffer()->getDepth();
            }

            if(graphicsService->setLinearFrameBuffer(buffer)) {
                graphicsService->getLinearFrameBuffer()->init(res.resX, res.resY, res.depth);
            }

            break;
        }
        default:
            break;
    }

    delete[] tmp;

    return numBytes;
}
