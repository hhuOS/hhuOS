#include "ZeroNode.h"

uint64_t ZeroNode::getLength() {
    return 0;
}

char *ZeroNode::readData(char *buf, uint64_t pos, uint32_t numBytes) {
    memset(buf, 0, numBytes);
    return buf;
}

int32_t ZeroNode::writeData(char *buf, uint64_t pos, uint32_t numBytes) {
    return -1;
}