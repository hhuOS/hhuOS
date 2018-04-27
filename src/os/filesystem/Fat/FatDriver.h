#ifndef __FatDriver_include__
#define __FatDriver_include__

#include "filesystem/FsDriver.h"
#include "filesystem/Fat/FatFsLib/FatFs.h"

extern "C" {
#include "lib/libc/string.h"
}

/**
 * An implementation of FsDriver for the Fat-library.
 */
class FatDriver : public FsDriver {
    
private:
    FatFs *fatInstance = nullptr;
    StorageDevice *device;

public:
    /**
     * Constructor.
     */
    FatDriver() = default;

    /**
     * Copy-constructor.
     */
    FatDriver(const FatDriver  &copy) = delete;

    /**
     * Destructor.
     */
    ~FatDriver() override;

    /**
     * Overriding virtual function from FsDriver.
     */
    bool createFs(StorageDevice *device) override;

    /**
     * Overriding virtual function from FsDriver.
     */
    bool mount(StorageDevice *device) override;

    /**
     * Overriding virtual function from FsDriver.
     */
    FsNode *getNode(const String &path) override;

    /**
     * Overriding virtual function from FsDriver.
     */
    bool createNode(const String &path, uint8_t fileType) override;

    /**
     * Overriding virtual function from FsDriver.
     */
    bool deleteNode(const String &path) override;
};

#endif