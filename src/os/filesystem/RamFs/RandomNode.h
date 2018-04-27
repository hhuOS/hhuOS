#ifndef __RandomNode_include__
#define __RandomNode_include__

#include "VirtualNode.h"
#include <cstdint>

/**
 * Implementation of VirtualNode, that always fills a buffer with random bytes in readData().
 *
 * @author Fabian Ruhland
 * @date 2017
 */
class RandomNode : public VirtualNode {

public:
    /**
     * Constructor.
     */
    RandomNode();

    /**
     * Copy-constructor.
     */
    RandomNode(const RandomNode &copy) = delete;

    /**
     * Destructor.
     */
    ~RandomNode() override = default;

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