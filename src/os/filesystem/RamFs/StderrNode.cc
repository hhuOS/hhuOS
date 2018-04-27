#include "StderrNode.h"
#include "kernel/Kernel.h"

StderrNode::StderrNode() : VirtualNode("stderr", FsNode::CHAR_FILE) {
    graphicsService = Kernel::getService<GraphicsService>();
    stdStreamService = Kernel::getService<StdStreamService>();
}

uint64_t StderrNode::getLength() {
    return 0;
}

uint64_t StderrNode::readData(char *buf, uint64_t pos, uint64_t numBytes) {
    return 0;
}

uint64_t StderrNode::writeData(char *buf, uint64_t pos, uint64_t numBytes) {
    OutputStream *stderr = stdStreamService->getStderr();

    stderr->writeBytes(buf, numBytes);
    stderr->flush();

    return numBytes;
}