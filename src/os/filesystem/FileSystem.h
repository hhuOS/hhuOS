/*
 * Copyright (C) 2018 Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schoettner
 * Heinrich-Heine University
 *
 * This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any
 * later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>
 */

#ifndef __FileSystem_include__
#define __FileSystem_include__

#include <cstdint>
#include "kernel/services/StorageService.h"
#include <filesystem/RamFs/VirtualNode.h>
#include <filesystem/FsDriver.h>
#include "kernel/services/EventBus.h"

/**
 * The filesystem. It works by maintaining a list of mount-points. Every request will be handled by picking the right
 * mount-point and and passing the request over to the corresponding FsDriver.
 */
class FileSystem : public KernelService, Receiver {

private:

    static Logger &log;

    Util::HashMap<String, FsDriver*> mountPoints;

    Spinlock fsLock;

    StorageService *storageService = nullptr;
    EventBus *eventBus = nullptr;

    /**
     * Get the driver, that is mounted at a specified path.
     * CAUTION: May return nullptr, if the file does not exist.
     *          Always check the return value!
     *
     * @param path The path. After successful execution, the part up to the mount point will truncated,
     *             so that the path can be used for the returned driver.
     *
     * @return The driver (or nullptr on failure)
     */
    FsDriver *getMountedDriver(String &path);

public:
    /**
     * Possible return codes.
     */
    enum RETURN_CODES {
        SUCCESS = 0x00,
        FILE_NOT_FOUND = 0x01,
        DEVICE_NOT_FOUND = 0x02,
        INVALID_DRIVER = 0x03,
        FORMATTING_FAILED = 0x04,
        MOUNT_TARGET_ALREADY_USED = 0x05,
        MOUNTING_FAILED = 0x06,
        ADDING_VIRTUAL_NODE_FAILED = 0x07,
        SUBDIRECTORY_CONTAINS_MOUNT_POINT = 0x08,
        NOTHING_MOUNTED_AT_PATH = 0x09,
        CREATING_FILE_FAILED = 0x10,
        CREATING_DIRECTORY_FAILED = 0x11,
        DELETING_FILE_FAILED = 0x12
    };

    /**
     * Constructor.
     */
    FileSystem();

    /**
     * Copy-constructor.
     */
    FileSystem(const FileSystem &copy) = delete;

    /**
     * Destructor.
     */
    ~FileSystem() override;

    /**
     * Processes the '.' and '..' entries of a path.
     *
     * @param path The path
     * @return The processed path
     */
    static String parsePath(const String &path);

    /**
     * Initialize the Filesystem.
     */
    void init();

    /**
     * Add a virtual node to a mounted RamFsDriver.
     * A RamFsDriver MUST be mounted at the location that the path points to.
     * Otherwise the behaviour of this function is undetermined.
     *
     * @param path The path
     * @param node The node
     *
     * @return Return code.
     */
    uint32_t addVirtualNode(const String &path, VirtualNode *node);

    /**
     * Format a device with a specified filesystem-type.
     *
     * @param devicePath The device-file (in /dev/storage) or the direct device name (e.g. "hdd0p1")
     * @param fsType The filesystem-type
     *
     * @return Return code
     */
    uint32_t createFilesystem(const String &devicePath, const String &fsType);

    /**
     * Mounts a device at a specified location.
     *
     * @param devicePath The device-file (in /dev/storage) or the direct device name (e.g. "hdd0p1")
     * @param targetPath The mount-path
     * @param fsType The filesystem-type
     *
     * @return Return code
     */
    uint32_t mount(const String &devicePath, const String &targetPath, const String &fsType);

    /**
     * Unmounts a device from a specified location.
     *
     * @param path The mount-path
     *
     * @return Return code.
     */
    uint32_t unmount(const String &path);

    /**
     * Get a node at a specified path.
     * CAUTION: May return nullptr, if the file does not exist.
     *          Always check the return value!
     *
     * @param path The path
     *
     * @return The node (or nullptr on failure)
     */
    FsNode *getNode(const String &path);

    /**
     * Create a file at a specified path.
     * The parent-directory of the new folder must exist beforehand.
     *
     * @param path The path
     *
     * @return Return code
     */
    uint32_t createFile(const String &path);

    /**
     * Create a directory at a specified path.
     * The parent-directory of the new folder must exist beforehand.
     *
     * @param path The path
     *
     * @return Return code
     */
    uint32_t createDirectory(const String &path);

    /**
     * Delete an existing file or directory at a given path.
     * The file must be a regular file or an empty folder (a leaf in the filesystem tree).
     *
     * @param path The path
     *
     * @return Return code
     */
    uint32_t deleteFile(const String &path);

    /**
     * Overriding function from Receiver.
     */
    void onEvent(const Event &event) override;


    static constexpr const char* SERVICE_NAME = "FileSystem";

    static constexpr const char *ROOT = "/";

    static constexpr const char *SEPARATOR = "/";

    static constexpr const char *TYPE_RAM_FS = "ramfs";
};

#endif
