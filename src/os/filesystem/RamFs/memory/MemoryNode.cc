//
// Created by burak on 09.04.18.
//

#include "MemoryNode.h"

uint64_t MemoryNode::getLength() {
    getValues();
    return cache.length();
}

uint64_t MemoryNode::readData(char *buf, uint64_t pos, uint64_t numBytes) {
    getValues();

    uint32_t length = cache.length();

    if (pos + numBytes > length) {
        numBytes = (uint64_t) (length - pos);
    }

    memcpy(buf, (char*) cache + pos, numBytes);

    return numBytes;
}

uint64_t MemoryNode::writeData(char *buf, uint64_t pos, uint64_t numBytes) {
    return 0;
}

MemoryNode::MemoryNode(String name, FsNode::FILE_TYPE type) : VirtualNode(name, type) {}
