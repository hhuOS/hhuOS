//
// Created by fabian on 01.07.18.
//

#include "CurrentResolutionNode.h"

CurrentResolutionNode::CurrentResolutionNode(GraphicsNode::GraphicsMode mode) : GraphicsNode("resolution", mode) {

}

uint64_t CurrentResolutionNode::writeData(char *buf, uint64_t pos, uint64_t numBytes) {
    String buffer(buf);
    uint16_t resX, resY;
    uint8_t depth;

    Util::Array<String> array1 = buffer.split("x");

    if(array1.length() != 2) {
        return 0;
    }

    Util::Array<String> array2 = array1[1].split("@");

    if(array2.length() != 2) {
        return 0;
    }

    resX = static_cast<uint16_t>(strtoint((const char *) array1[0]));
    resY = static_cast<uint16_t>(strtoint((const char *) array2[0]));
    depth = static_cast<uint8_t>(strtoint((const char *) array2[1]));

    switch(mode) {
        case TEXT : {
            TextDriver *driver = graphicsService->getTextDriver();

            driver->init(resX, resY, depth);

            break;
        }
        case LINEAR_FRAME_BUFFER : {
            LinearFrameBuffer *lfb = graphicsService->getLinearFrameBuffer();

            lfb->init(resX, resY, depth);

            break;
        }
        default:
            break;
    }

    return numBytes;
}

void CurrentResolutionNode::writeValuesToCache() {
    switch(mode) {
        case TEXT : {
            TextDriver *driver = graphicsService->getTextDriver();

            cache += String::valueOf(driver->getColumnCount(), 10) + String("x") +
                     String::valueOf(driver->getRowCount(), 10) + String("@") +
                     String::valueOf(driver->getDepth(), 10) + String("\n");

            break;
        }
        case LINEAR_FRAME_BUFFER : {
            LinearFrameBuffer *lfb = graphicsService->getLinearFrameBuffer();

            cache += String::valueOf(lfb->getResX(), 10) + String("x") +
                     String::valueOf(lfb->getResY(), 10) + String("@") +
                     String::valueOf(lfb->getDepth(), 10) + String("\n");
        }
        default:
            break;
    }
}
