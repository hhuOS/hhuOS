#ifndef __FileSystem_include__
#define __FileSystem_include__

#include <cstdint>
#include "StorageService.h"
#include <kernel/filesystem/RamFs/VirtualNode.h>
#include <kernel/filesystem/FsDriver.h>
#include "EventBus.h"

/**
 * The filesystem. It works by maintaining a list of mount-points. Every request will be handled by picking the right
 * mount-point and and passing the request over to the corresponding FsDriver.
 */
class FileSystem : public KernelService, Receiver {

private:
    Util::HashMap<String, FsDriver*> mountPoints;

    Spinlock fsLock;

    //StorageService *storageService = nullptr;
    EventBus *eventBus = nullptr;

public:
    enum RETURN_CODES {
        SUCCESS = 0x00,
        FILE_NOT_FOUND = 0x01,
        DEVICE_NOT_FOUND = 0x02,
        INVALID_DRIVER = 0x03,
        FORMATTING_FAILED = 0x04,
        MOUNT_TARGET_ALREADY_USED = 0x05,
        MOUNTING_FAILED = 0x06,
        ADDING_VIRTUAL_NODE_FAILED = 0x07,
        NOTHING_MOUNTED_AT_PATH = 0x08,
        CREATING_FILE_FAILED = 0x09,
        CREATING_DIRECTORY_FAILED = 0x10,
        DELETING_FILE_FAILED = 0x11
    };

    FileSystem();

    FileSystem(const FileSystem &copy) = delete;

    ~FileSystem() override;

    FsDriver *getMountedDriver(String &path);

    static String parsePath(const String &path);

    void init();

    uint32_t addVirtualNode(const String &path, VirtualNode *node);

    uint32_t createFilesystem(const String &path, const String &fsType);

    uint32_t mount(const String &devicePath, const String &targetPath, const String &type);

    uint32_t unmount(const String &path);

    FsNode *getNode(const String &path);

    uint32_t createFile(const String &path);

    uint32_t createDirectory(const String &path);

    uint32_t deleteFile(const String &path);


    void onEvent(const Event &event) override ;


    static constexpr const char* SERVICE_NAME = "FileSystem";

    static constexpr const char *ROOT = "/";

    static constexpr const char *SEPARATOR = "/";

    static constexpr const char *TYPE_FAT = "fat";

    static constexpr const char *TYPE_RAM = "ram";
};

#endif
