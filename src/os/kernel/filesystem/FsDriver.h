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

public:
    /**
     * Constructor.
     */
    FsDriver() = default;

    /**
     * Copy-constructor.
     */
    FsDriver(const FsDriver  &copy) = delete;

    /**
     * Destructor.
     */
    virtual ~FsDriver() = default;

    /**
     * Mount a device.
     * After this function has succeeded, the driver must be ready to answer process requests for this device.
     *
     * @param device The device
     *
     * @return true, on success
     */
    virtual bool mount(StorageDevice *device) = 0;

    /**
     * Format a device.
     *
     * @param device The device
     *
     * @return true, on success
     */
    virtual bool makeFs(StorageDevice *device) = 0;

    /**
     * Get an FsNode, representing a file or directory that a given path points to.
     * 
     * @param path The path.
     * 
     * @return The FsNode (or nulltpr on failure)
     */ 
    virtual FsNode *getNode(const String &path) = 0;

    /**
     * Create a new empty file or directory at a given path.
     * The parent-directory of the new file must exist beforehand.
     * 
     * @param path The path
     * @param fileType The filetype
     * 
     * @return true on success.
     */
    virtual bool createNode(const String &path, uint8_t fileType) = 0;

    /**
     * Delete an existing file or directory at a given path.
     * The file must be a regular file or an empty folder (a leaf in the filesystem tree).
     * 
     * @param path The path.
     * 
     * @return true on success.
     */
    virtual bool deleteNode(const String &path) = 0;
};

#endif