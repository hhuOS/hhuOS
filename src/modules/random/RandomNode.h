#ifndef __RandomNode_include__
#define __RandomNode_include__

#include "kernel/filesystem/RamFs/VirtualNode.h"
#include <stdint.h>

class RandomNode : public VirtualNode {
private:
    RandomNode(const RandomNode &copy);

public:
    RandomNode() : VirtualNode("random", CHAR_FILE) {};
    ~RandomNode() {}

    uint64_t getLength();
    char *readData(char *buf, uint64_t pos, uint32_t numBytes);
    int32_t writeData(char *buf, uint64_t pos, uint32_t numBytes);
};

#endif