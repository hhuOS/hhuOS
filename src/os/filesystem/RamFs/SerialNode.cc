#include "SerialNode.h"

String SerialNode::generateName(Serial::ComPort port) {
    switch(port) {
        case Serial::COM1 :
            return "serial1";
        case Serial::COM2 :
            return "serial2";
        case Serial::COM3 :
            return "serial3";
        case Serial::COM4 :
            return "serial4";
        default:
            return "serial";
    }
}

SerialNode::SerialNode(Serial *serial) : VirtualNode(generateName(serial->getPortNumber()), FsNode::BLOCK_FILE), serial(serial) {

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
