#ifndef __ZeroNode_include__
#define __ZeroNode_include__

#include "kernel/filesystem/RamFs/VirtualNode.h"
#include <cstdint>

class ZeroNode : public VirtualNode {

public:
    /**
     * Constructor.
     */
    ZeroNode();

    /**
     * Copy-constructor.
     */
    ZeroNode(const ZeroNode &copy) = delete;

    /**
     * Destructor.
     */
    ~ZeroNode() override = default;

    /**
     * Overriding function from VirtualNode.
     */
    uint64_t getLength() override;

    /**
     * Overriding function from VirtualNode.
     */
    char *readData(char *buf, uint64_t pos, uint32_t numBytes) override;

    /**
     * Overriding function from VirtualNode.
     */
    int32_t writeData(char *buf, uint64_t pos, uint32_t numBytes) override;
};

#endif