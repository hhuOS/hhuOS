#ifndef __StorageNode_include__
#define __StorageNode_include__

#include "VirtualNode.h"
#include "devices/block/storage/StorageDevice.h"
#include <stdint.h>

/**
 * Implementation of VirtualNode, that allows bytewise reading from and writing to a StorageDevice.
 *
 * @author Fabian Ruhland
 * @date 2017
 */
class StorageNode : public VirtualNode {

private:
    StorageDevice *disk;

public:
    /**
     * Default-constructor.
     */
    StorageNode() = delete;

    /**
     * Constructor.
     * @param disk The storage device
     */
    explicit StorageNode(StorageDevice *disk);

    /**
     * Copy-constructor.
     */
    StorageNode(const StorageNode &copy) = delete;

    /**
     * Destructor.
     */
    ~StorageNode() override = default;

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