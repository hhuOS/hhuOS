#include "StderrNode.h"
#include "kernel/Kernel.h"

StderrNode::StderrNode() : VirtualNode("stderr", CHAR_FILE) {
    stdStreamService = Kernel::getService<StdStreamService>();
}

uint64_t StderrNode::getLength() {
    return 0;
}

bool StderrNode::readData(char *buf, uint64_t pos, uint64_t numBytes) {
    memset(buf, 0, numBytes);

    return true;
}

bool StderrNode::writeData(char *buf, uint64_t pos, uint64_t numBytes) {
    OutputStream *stderr = stdStreamService->getStderr();
    
    *stderr << buf;
    stderr->flush();

    return true;
}