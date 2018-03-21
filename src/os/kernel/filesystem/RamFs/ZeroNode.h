#ifndef __ZeroNode_include__
#define __ZeroNode_include__

#include "kernel/filesystem/RamFs/VirtualNode.h"
#include <cstdint>

/**
 * Implementation of VirtualNode, that always fills a buffer with zeros in readData().
 */
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
    uint64_t readData(char *buf, uint64_t pos, uint64_t numBytes) override;

    /**
     * Overriding function from VirtualNode.
     */
    uint64_t writeData(char *buf, uint64_t pos, uint64_t numBytes) override;
};

#endif