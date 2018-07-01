#ifndef HHUOS_CURRENTRESOLUTIONNODE_H
#define HHUOS_CURRENTRESOLUTIONNODE_H

#include "GraphicsNode.h"

/**
 * Implementation of GraphicsNode, that contains the current resolution.
 * Writing to this node will result in a resolution change.
 *
 * @author Fabian Ruhland
 * @date 2018
 */
class CurrentResolutionNode : public GraphicsNode {

public:

    /**
     * Constructor.
     */
    explicit CurrentResolutionNode(GraphicsNode::GraphicsMode mode);

    /**
     * Copy-Constructor.
     */
    CurrentResolutionNode(const CurrentResolutionNode &copy) = delete;

    /**
     * Destructor.
     */
    ~CurrentResolutionNode() override = default;

    /**
     * Overriding function from GraphicsNode.
     */
    uint64_t writeData(char *buf, uint64_t pos, uint64_t numBytes) override;

    /**
     * Overriding function from GraphicsNode.
     */
    void writeValuesToCache() override;
};

#endif
