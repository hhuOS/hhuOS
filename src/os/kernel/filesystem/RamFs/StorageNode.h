#ifndef __StorageNode_include__
#define __StorageNode_include__

#include "VirtualNode.h"
#include "devices/block/storage/StorageDevice.h"
#include <stdint.h>

class StorageNode : public VirtualNode {
private:
    StorageNode(const StorageNode &copy);

    StorageDevice *disk;

public:
    StorageNode(StorageDevice *arg_disk) : VirtualNode(arg_disk->getName(), BLOCK_FILE){
        disk = arg_disk;
    };
    ~StorageNode() {}

    uint64_t getLength();
    uint64_t readData(char *buf, uint64_t pos, uint64_t numBytes);
    uint64_t writeData(char *buf, uint64_t pos, uint64_t numBytes);
};

#endif