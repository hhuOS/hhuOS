#include "RandomNode.h"
#include "lib/Random.h"

RandomNode::RandomNode() : VirtualNode("random", FsNode::CHAR_FILE) {

}

uint64_t RandomNode::getLength() {
    return 0;
}

uint64_t RandomNode::readData(char *buf, uint64_t pos, uint64_t numBytes) {
    auto *random = new Random(0xFF);
    
    for(uint32_t i = 0; i < numBytes; i++) {
        buf[i] = (char) random->rand();
    }
    
    delete random;
    return numBytes;
}

uint64_t RandomNode::writeData(char *buf, uint64_t pos, uint64_t numBytes) {
    return 0;
}