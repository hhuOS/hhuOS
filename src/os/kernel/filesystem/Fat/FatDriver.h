#ifndef __FatDriver_include__
#define __FatDriver_include__

#include "../FsDriver.h"
#include "FatFsLib/FatFs.h"

extern "C" {
#include "lib/libc/string.h"
}

/**
 * An implementation of FsDriver for the Fat-library.
 */
class FatDriver : public FsDriver {
    
private:
    FatDriver(const FatDriver  &copy);

    FatFs *fatInstance = nullptr;
    StorageDevice *device;

public:
    FatDriver() {};
    ~FatDriver() {
        if(fatInstance != nullptr)
            delete fatInstance;
    }

    bool makeFs(StorageDevice *device);

    bool mount(StorageDevice *device);
    FsNode *getNode(const String &path);
    bool createNode(const String &path, uint8_t fileType);
    bool deleteNode(const String &path);
};

#endif