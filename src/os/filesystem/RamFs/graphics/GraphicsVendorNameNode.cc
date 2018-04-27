#include <kernel/Kernel.h>
#include <devices/graphics/text/TextDriver.h>
#include "GraphicsVendorNameNode.h"

GraphicsVendorNameNode::GraphicsVendorNameNode(uint8_t mode) : VirtualNode("vendor", FsNode::REGULAR_FILE), mode(mode){
    graphicsService = Kernel::getService<GraphicsService>();
}

uint64_t GraphicsVendorNameNode::getLength() {
    switch(mode) {
        case TEXT :
           return graphicsService->getTextDriver()->getVendorName().length() + 1;
        case LINEAR_FRAME_BUFFER :
            return graphicsService->getLinearFrameBuffer()->getVendorName().length() + 1;
        default:
            return 0;
    }
}

uint64_t GraphicsVendorNameNode::readData(char *buf, uint64_t pos, uint64_t numBytes) {
    String name;

    switch(mode) {
        case TEXT :
            name = graphicsService->getTextDriver()->getVendorName() + "\n";
            break;
        case LINEAR_FRAME_BUFFER :
            name = graphicsService->getLinearFrameBuffer()->getVendorName() + "\n";
            break;
        default:
            break;
    }

    uint64_t length = name.length();

    if (pos + numBytes > length) {
        numBytes = (uint32_t) (length - pos);
    }

    memcpy(buf, (char*) name + pos, numBytes);

    return numBytes;
}

uint64_t GraphicsVendorNameNode::writeData(char *buf, uint64_t pos, uint64_t numBytes) {
    return 0;
}