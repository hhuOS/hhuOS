#ifndef __RamFsDriver_include__
#define __RamFsDriver_include__

#include "filesystem/FsDriver.h"
#include "VirtualNode.h"

/**
 * An implementation of FsDriver for a file system, that exists completely in memory.
 *
 * @author Fabian Ruhland
 * @date 2017
 */
class RamFsDriver : public FsDriver {

private:
    /**
     * Get the child of a VirtualNode for its name.
     * CAUTION: May return nullptr, if no child with the specified name is found.
     *
     * @param parent The node
     * @param path The child's name
     * @return The child (or nullptr on failure)
     */
    static VirtualNode *getChildByName(VirtualNode *parent, const String &path);

protected:
    VirtualNode *rootNode = nullptr;

public:
    /**
     * Constructor.
     */
    RamFsDriver() = default;

    /**
     * Copy-constructor.
     */
    RamFsDriver(const RamFsDriver &copy) = delete;

    /**
     * Destructor.
     */
    ~RamFsDriver() override;

    /**
     * Overriding function from FsDriver.
     */
    bool createFs(StorageDevice *device) override;

    /**
     * Overriding function from FsDriver.
     */
    bool mount(StorageDevice *device) override;

    /**
     * Overriding function from FsDriver.
     */
    FsNode *getNode(const String &path) override;

    /**
     * Overriding function from FsDriver.
     */
    bool createNode(const String &path, uint8_t fileType) override;

    /**
     * Overriding function from FsDriver.
     */
    bool deleteNode(const String &path) override;

    bool addNode(const String &path, VirtualNode *node);
};

#endif
