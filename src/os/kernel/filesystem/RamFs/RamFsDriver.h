#ifndef __RamFsDriver_include__
#define __RamFsDriver_include__

#include "../FsDriver.h"
#include "VirtualNode.h"

/**
 * An implementation of FsDriver for a file system, that exists completely in memory.
 */
class RamFsDriver : public FsDriver {

public:

    RamFsDriver() {};

    ~RamFsDriver() {
        if(rootNode != nullptr) {
            delete rootNode;
        }
    }
    
    int32_t makeFs(StorageDevice *device);

    int32_t mount(StorageDevice *device);

    FsNode *getNode(const String &path);

    int32_t addNode(const String &path, VirtualNode *node);

    int32_t createNode(const String &path, uint8_t fileType);

    int32_t deleteNode(const String &path);

protected:

    VirtualNode *rootNode = nullptr;

private:

    RamFsDriver(const RamFsDriver  &copy);
};

#endif
