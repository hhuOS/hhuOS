#ifndef __FileSystem_include__
#define __FileSystem_include__

#include "kernel/filesystem/FsDriver.h"
#include "kernel/filesystem/RamFs/VirtualNode.h"
#include "kernel/Spinlock.h"
#include "kernel/Kernel.h"
#include "kernel/services/EventBus.h"
#include "kernel/events/Receiver.h"
#include "kernel/events/storage/StorageAddEvent.h"
#include "kernel/events/storage/StorageRemoveEvent.h"


#include <cstdint>

/**
 * The filesystem. This class is instantiated only once in the Globals-file.
 * It maintains a list of mount-points. Every request will be handled by picking the right
 * mount-point and and passing the request over to the corresponding FsDriver.
 */
class FileSystem : public KernelService, Receiver {

public:

    FileSystem(const FileSystem &copy) = delete;

    /**
     * Represents a mount-point.
     * It consists of the mount-path and a pointer to the corresponding FsDriver.
     */
    struct MountPoint {
        String path;
        FsDriver *driver;
    };

    struct MountInfo {
        MountPoint *mountPoint;
        String pathInMount;

        MountInfo(MountPoint *mountPoint, const String &pathInMount);
    };

    FileSystem() = default;

    ~FileSystem() {
        for(uint32_t i = 0; i < mountPoints.length(); i++) {
            MountPoint *currentMountPoint = mountPoints.get(i);
            mountPoints.remove(currentMountPoint);
            delete currentMountPoint->driver;
            delete currentMountPoint;
        }

        EventBus *eventBus = (EventBus*) Kernel::getService(EventBus::SERVICE_NAME);
        eventBus->unsubscribe(*this, StorageAddEvent::TYPE);
        eventBus->unsubscribe(*this, StorageRemoveEvent::TYPE);
    }

    MountInfo getMountInfo(const String &path);

    static String parsePath(const String &path);

    void init();

    int32_t addVirtualNode(const String &path, VirtualNode *node);

    int32_t createFilesystem(const String &path, const String &fsType);

    int32_t mount(const String &device, const String &path, const String &type);

    int32_t unmount(const String &path);

    FsNode *getNode(const String &path);

    int32_t createFile(const String &path);

    int32_t createDirectory(const String &path);

    int32_t deleteFile(const String &path);


    void onEvent(const Event &event) override ;


    static constexpr char* SERVICE_NAME = "FileSystem";

    static constexpr const char *ROOT = "/";

    static constexpr const char *SEPARATOR = "/";

    static constexpr const char *TYPE_FAT = "fat";

    static constexpr const char *TYPE_RAM = "ram";

    static constexpr const char *TYPE_DEV = "dev";

private:

    LinkedList<MountPoint> mountPoints;

    Spinlock fsLock;
};

#endif
