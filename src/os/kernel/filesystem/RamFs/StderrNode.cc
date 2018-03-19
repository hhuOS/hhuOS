#include "StderrNode.h"
#include "kernel/Kernel.h"

StderrNode::StderrNode() : VirtualNode("stderr", CHAR_FILE) {
    stdStreamService = (StdStreamService *) Kernel::getService(StdStreamService::SERVICE_NAME);
}

uint64_t StderrNode::getLength() {
    return 0;
}

char *StderrNode::readData(char *buf, uint64_t pos, uint32_t numBytes) {
    memset(buf, 0, numBytes);

    return buf;
}

int32_t StderrNode::writeData(char *buf, uint64_t pos, uint32_t numBytes) {
    OutputStream *stderr = stdStreamService->getStderr();
    
    *stderr << buf;
    stderr->flush();

    return 0;
}