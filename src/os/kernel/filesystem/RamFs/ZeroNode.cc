#include "ZeroNode.h"

ZeroNode::ZeroNode() : VirtualNode("zero", CHAR_FILE) {

}

uint64_t ZeroNode::getLength() {
    return 0;
}

char *ZeroNode::readData(char *buf, uint64_t pos, uint64_t numBytes) {
    memset(buf, 0, numBytes);
    return buf;
}

int64_t ZeroNode::writeData(char *buf, uint64_t pos, uint64_t numBytes) {
    return -1;
}