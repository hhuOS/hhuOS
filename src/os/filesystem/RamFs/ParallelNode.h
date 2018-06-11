#ifndef HHUOS_PARALLELNODE_H
#define HHUOS_PARALLELNODE_H

#include <kernel/services/ParallelService.h>
#include <kernel/Kernel.h>
#include "VirtualNode.h"

/**
 * Implementation of VirtualNode, that writes to an LPT-Port.
 *
 * @author Fabian Ruhland
 * @date 2018
 */
class ParallelNode : public VirtualNode {

private:

    Parallel *parallel = nullptr;

    static String generateName(Parallel::LptPort port);

public:
    /**
     * Constructor.
     */
    explicit ParallelNode(Parallel *parallel);

    /**
     * Copy-constructor.
     */
    ParallelNode(const ParallelNode &copy) = delete;

    /**
     * Destructor.
     */
    ~ParallelNode() override = default;

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
