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