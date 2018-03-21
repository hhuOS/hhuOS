#include "RamFsNode.h"

String RamFsNode::getName() {
    return node->getName();
}

uint8_t RamFsNode::getFileType() {
    return node->getFileType();
}

uint64_t RamFsNode::getLength() {
    return node->getLength();
}

Util::Array<String> RamFsNode::getChildren() {
    Util::ArrayList<VirtualNode*> &children = node->getChildren();
    Util::Array<String> ret(children.size());

    for(uint32_t i = 0; i < children.size(); i++) {
        ret[i] = children.get(i)->getName();
    }

    return ret;
}

uint64_t RamFsNode::readData(char *buf, uint64_t pos, uint64_t numBytes) {
    uint64_t readBytes = node->readData(buf, pos, numBytes);

    if(readBytes < numBytes) {
        buf[readBytes] = VFS_EOF;
    }

    return readBytes;
}

uint64_t RamFsNode::writeData(char *buf, uint64_t pos, uint64_t numBytes) {
    return node->writeData(buf, pos, numBytes);
}