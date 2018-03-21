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

/**
 * Returns the node's name.
 * 
 * @return The node's name.
 */
String VirtualNode::getName() {
    return name;
}

/**
 * Returns the node's filetype;
 * 
 * @return The node's filetype.
 */
uint8_t VirtualNode::getFileType() {
    return fileType;
}

/**
 * Returns the length (in bytes) of the node's data.
 * If the node is a directory, it always returns 0.
 * 
 * @return The length (in bytes) of the node's data.
 */
uint64_t VirtualNode::getLength() {
    return length;
}

/**
 * Returns the name of the node's child at index 'pos' (0->First child, 1->second child, ...).
 * 
 * @param pos The child's index.
 * 
 * @return The node's children.
 */
Util::ArrayList<VirtualNode*> &VirtualNode::getChildren() {
    return children;
}

/**
 * Reads bytes from the node's data.
 * 
 * @param pos The offset, at which the data shall be read.
 *            May be ignored for some special files (e.g. senseless for /dev/zero).
 * @param numBytes The amount of bytes, that shall be read.
 * 
 * @return The data.
 */
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

/**
 * Writes bytes to the node's data. If the offset points right into the existing data,
 * it shall be overwritten with the new data. If the new data does not fit, the data size will increase.
 * 
 * @param buf The data, that shall be written to the node.
 * @param pos The offset, at which the new data shall be written.
 * @param numBytes The new data's length (in bytes).
 * 
 * @return 0, if the data has been written succesfully.
 */
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