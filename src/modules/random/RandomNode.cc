#include "RandomNode.h"
#include "lib/Random.h"

uint64_t RandomNode::getLength() {
    return 0;
}

char *RandomNode::readData(char *buf, uint64_t pos, uint32_t numBytes) {
    Random *random = new Random(0xFF);
    
    for(uint32_t i = 0; i < numBytes; i++) {
        buf[i] = (char) random->rand();
    }
    
    delete random;
    return buf;
}

int32_t RandomNode::writeData(char *buf, uint64_t pos, uint32_t numBytes) {
    return -1;
}