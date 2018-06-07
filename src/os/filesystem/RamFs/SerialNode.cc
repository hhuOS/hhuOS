#include "SerialNode.h"


SerialNode::SerialNode(Serial *serial) : VirtualNode("serial", FsNode::BLOCK_FILE), serial(serial) {

}

uint64_t SerialNode::getLength() {
    return 0;
}

uint64_t SerialNode::readData(char *buf, uint64_t pos, uint64_t numBytes) {
    serial->readData(buf, static_cast<uint32_t>(numBytes));

    // Convert carriage returns to '\n'
    for(uint32_t i = 0; i < numBytes; i++) {
        if(buf[i] == 13) {
            buf[i] = '\n';
        }
    }

    return numBytes;
}

uint64_t SerialNode::writeData(char *buf, uint64_t pos, uint64_t numBytes) {
    serial->sendData(buf, static_cast<uint32_t>(numBytes));

    return numBytes;
}