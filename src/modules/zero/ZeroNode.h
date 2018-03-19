#ifndef __ZeroNode_include__
#define __ZeroNode_include__

#include "../../os/kernel/filesystem/RamFs/VirtualNode.h"
#include <stdint.h>

class ZeroNode : public VirtualNode {
private:
    ZeroNode(const ZeroNode &copy);

public:
    ZeroNode() : VirtualNode("zero", CHAR_FILE) {};
    ~ZeroNode() {}

    uint64_t getLength();
    char *readData(char *buf, uint64_t pos, uint32_t numBytes);
    int32_t writeData(char *buf, uint64_t pos, uint32_t numBytes);
};

#endif