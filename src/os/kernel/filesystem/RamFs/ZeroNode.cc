#include "ZeroNode.h"

ZeroNode::ZeroNode() : VirtualNode("zero", FsNode::CHAR_FILE) {

}

uint64_t ZeroNode::getLength() {
    return 0;
}

uint64_t ZeroNode::readData(char *buf, uint64_t pos, uint64_t numBytes) {
    memset(buf, 0, static_cast<size_t>(numBytes));

    return numBytes;
}

uint64_t ZeroNode::writeData(char *buf, uint64_t pos, uint64_t numBytes) {
    return 0;
}