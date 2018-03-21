#include "VirtualNode.h"

VirtualNode::VirtualNode(const String &name, uint8_t fileType) : name(name), fileType(fileType), length(0) {

}

VirtualNode::~VirtualNode() {
    for(const auto &elemement : children) {
        delete elemement;
    }

    if(data != nullptr) {
        delete data;
    }
}

String VirtualNode::getName() {
    return name;
}

uint8_t VirtualNode::getFileType() {
    return fileType;
}

uint64_t VirtualNode::getLength() {
    return length;
}

Util::ArrayList<VirtualNode*> &VirtualNode::getChildren() {
    return children;
}

uint64_t VirtualNode::readData(char *buf, uint64_t pos, uint64_t numBytes) {
    if(pos >= length) {
        return 0;
    }

    if (pos + numBytes > length) {
        numBytes = (length - pos);
    }

    for(uint64_t i = 0; i < numBytes; i++) {
        buf[i] = data[pos + i];
    }
    
    return numBytes;
}

uint64_t VirtualNode::writeData(char *buf, uint64_t pos, uint64_t numBytes) {
    if(pos + numBytes >= length) {
        auto *newData = new char[pos + numBytes];

        memset(newData, 0, pos + numBytes);
        memcpy(newData, data, length);

        delete data;
        data = newData;

        length = pos + numBytes;
    }

    for(uint32_t i = 0; i < numBytes; i++) {
        data[i + pos] = buf[i];
    }
    
    return numBytes;
}