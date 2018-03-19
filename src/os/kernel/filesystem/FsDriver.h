#ifndef __FsDriver_include__
#define __FsDriver_include__

#include "FsNode.h"
#include "devices/block/storage/StorageDevice.h"

/**
 * An interface between the FileSystem-class and a filesystem-driver.
 * Every filesystem-driver needs to implement implement the functions,
 * that are specified by this class. The FileSystem-class can then communicate
 * with the respective driver.
 */
class FsDriver {
    
private:
    FsDriver(const FsDriver  &copy);

public:
    FsDriver() {};
    virtual ~FsDriver() {};

    virtual int32_t mount(StorageDevice *device) = 0;

    virtual int32_t makeFs(StorageDevice *device) = 0;

    /**
     * Gets a FsNode, representing a file or directory that a given path points to.
     * 
     * @param path The path.
     * 
     * @return The FsNode or nullptr, if the path is invalid.
     */ 
    virtual FsNode *getNode(const String &path) = 0;

    /**
     * Creates a new empty file at a given path.
     * The parent-folder of the new file must exist beforehand.
     * 
     * @param path The new file's path.
     * 
     * @return 0 on success.
     */
    virtual int32_t createNode(const String &path, uint8_t fileType) = 0;

    /**
     * Deletes an existing file at a given path.
     * The file must be a regular file or an empty folder (a leaf in the filesystem tree).
     * 
     * @param path The file to be deleted.
     * 
     * @return 0 on success.
     */
    virtual int32_t deleteNode(const String &path) = 0;
};

#endif