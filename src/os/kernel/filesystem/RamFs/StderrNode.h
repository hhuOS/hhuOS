#ifndef __StderrNode_include__
#define __StderrNode_include__

#include "VirtualNode.h"
#include <cstdint>
#include <kernel/services/StdStreamService.h>
#include <kernel/services/GraphicsService.h>

/**
 * Implementation of VirtualNode, that always writes to the current standard error stream
 * (specified by StdStreamService).
 *
 * @author Fabian Ruhland
 * @date 2018
 */
class StderrNode : public VirtualNode {

private:
    GraphicsService *graphicsService = nullptr;
    StdStreamService *stdStreamService = nullptr;

public:
    /**
     * Constructor.
     */
    StderrNode();

    /**
     * Copy-constructor.
     */
    StderrNode(const StderrNode &copy) = delete;

    /**
     * Destructor.
     */
    ~StderrNode() override = default;

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