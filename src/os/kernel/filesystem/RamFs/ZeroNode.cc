#include "ZeroNode.h"

ZeroNode::ZeroNode() : VirtualNode("zero", CHAR_FILE) {

}

uint64_t ZeroNode::getLength() {
    return 0;
}

bool ZeroNode::readData(char *buf, uint64_t pos, uint64_t numBytes) {
    memset(buf, 0, static_cast<size_t>(numBytes));

    return true;
}

bool ZeroNode::writeData(char *buf, uint64_t pos, uint64_t numBytes) {
    return false;
}