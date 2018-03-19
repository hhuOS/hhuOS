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

    int32_t makeFs(StorageDevice *device);

    int32_t mount(StorageDevice *device);
    FsNode *getNode(const String &path);
    int32_t createNode(const String &path, uint8_t fileType);
    int32_t deleteNode(const String &path);
};

#endif