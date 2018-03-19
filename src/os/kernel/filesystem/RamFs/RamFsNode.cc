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

String RamFsNode::getChild(uint32_t pos) {
    return node->getChild(pos);
}

char *RamFsNode::readData(char *buf, uint64_t pos, uint32_t numBytes) {
    return node->readData(buf, pos, numBytes);
}

int32_t RamFsNode::writeData(char *buf, uint64_t pos, uint32_t numBytes) {
    return node->writeData(buf, pos, numBytes);
}