#include "StdoutNode.h"
#include "kernel/Kernel.h"

StdoutNode::StdoutNode() : VirtualNode("stdout", CHAR_FILE) {
    stdStreamService = Kernel::getService<StdStreamService>();
}

uint64_t StdoutNode::getLength() {
    return 0;
}

bool StdoutNode::readData(char *buf, uint64_t pos, uint64_t numBytes) {
    memset(buf, 0, numBytes);
    
    return true;
}

bool StdoutNode::writeData(char *buf, uint64_t pos, uint64_t numBytes) {
    OutputStream *stdout = stdStreamService->getStdout();

    char printBuf[numBytes + 1];
    memcpy(printBuf, buf, numBytes);
    printBuf[numBytes] = 0;
    
    *stdout << printBuf;
    stdout->flush();

    return true;
}