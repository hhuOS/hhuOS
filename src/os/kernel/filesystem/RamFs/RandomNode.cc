#include "RandomNode.h"
#include "lib/Random.h"

RandomNode::RandomNode() : VirtualNode("random", CHAR_FILE) {

}

uint64_t RandomNode::getLength() {
    return 0;
}

char *RandomNode::readData(char *buf, uint64_t pos, uint64_t numBytes) {
    auto *random = new Random(0xFF);
    
    for(uint32_t i = 0; i < numBytes; i++) {
        buf[i] = (char) random->rand();
    }
    
    delete random;
    return buf;
}

int64_t RandomNode::writeData(char *buf, uint64_t pos, uint64_t numBytes) {
    return -1;
}