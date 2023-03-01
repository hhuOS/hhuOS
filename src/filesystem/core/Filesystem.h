/*
 * Copyright (C) 2018-2023 Heinrich-Heine-Universitaet Duesseldorf,
 * Institute of Computer Science, Department Operating Systems
 * Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schoettner
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

#ifndef HHUOS_FILESYSTEM_H
#define HHUOS_FILESYSTEM_H

#include "lib/util/async/ReentrantSpinlock.h"
#include "lib/util/collection/HashMap.h"
#include "lib/util/collection/Array.h"
#include "lib/util/collection/Collection.h"
#include "lib/util/collection/Iterator.h"
#include "lib/util/base/String.h"
#include "filesystem/core/Driver.h"

namespace Filesystem {
class Node;
class VirtualDriver;
namespace Memory {
class MemoryDriver;
}  // namespace Memory


struct MountInformation {
    Util::String device;
    Util::String target;
    Util::String driver;

    bool operator!=(const MountInformation &other) const;
};

/**
 * The filesystem. It works by maintaining a list of mount points.
 * Every request is handled by picking the right mount point and and passing the request over to the corresponding driver.
 */
class Filesystem {

public:

    /**
     * Constructor.
     */
    Filesystem() = default;

    /**
     * Copy Constructor.
     */
    Filesystem(const Filesystem &copy) = delete;

    /**
     * Assignment operator.
     */
     Filesystem& operator=(const Filesystem &other) = delete;

    /**
     * Destructor.
     */
    ~Filesystem() = default;

    /**
     * Mounts a device at a specified location.
     *
     * @param deviceName The device name (e.g. "hdd0p1")
     * @param targetPath The mount path
     * @param driverName The filesystem driver name
     *
     * @return true on success
     */
    bool mount(const Util::String &deviceName, const Util::String &targetPath, const Util::String &driverName);

    /**
     * Mount a virtual filesystem at a specified location.
     *
     * @param targetPath The mount path
     * @param driver The virtual filesystem driver
     *
     * @return true on success
     */
    bool mountVirtualDriver(const Util::String &targetPath, VirtualDriver *driver);

    /**
     * Unmount a device from a specified location.
     *
     * @param path The mountVirtualDriver-path
     *
     * @return true on success
     */
    bool unmount(const Util::String &path);

    /**
     * Format a device with a specified filesystem type.
     *
     * @param deviceName The device name (e.g. "hdd0p1")
     * @param driverName The filesystem driver name
     *
     * @return Return code
     */
    bool createFilesystem(const Util::String &deviceName, const Util::String &driverName);

    /**
     * Get a node at a specified path.
     * CAUTION: May return nullptr, if the file does not exist.
     *          Always check the return value!
     *
     * @param path The path
     *
     * @return The node (or nullptr on failure)
     */
    Node* getNode(const Util::String &path);

    /**
     * Create a file at a specified path.
     * The parent-directory of the new folder must exist beforehand.
     *
     * @param path The path
     *
     * @return true on success
     */
    bool createFile(const Util::String &path);

    /**
     * Create a directory at a specified path.
     * The parent-directory of the new folder must exist beforehand.
     *
     * @param path The path
     *
     * @return true on success
     */
    bool createDirectory(const Util::String &path);

    /**
     * Delete an existing file or directory at a given path.
     * The file must be a regular file or an empty folder (a leaf in the filesystem ls).
     *
     * @param path The path
     *
     * @return true on success
     */
    bool deleteFile(const Util::String &path);

    /**
     * Get the virtual driver, that is mounted at a specified path.
     *
     * @param path The path
     *
     * @return The driver
     */
    [[nodiscard]] Memory::MemoryDriver& getVirtualDriver(const Util::String &path);

    /**
     * Get information about all mount points
     *
     * @return
     */
    [[nodiscard]] Util::Array<MountInformation> getMountInformation();
    
private:
    /**
     * Get the driver, that is mounted at a specified path.
     * The path needs to be absolute.
     * CAUTION: May return nullptr, if the file does not exist.
     *          Always check the return value!
     *
     * @param path The path. After successful execution, the part up to the mount point will be truncated,
     *             so that the path can be used for the returned driver.
     *
     * @return The driver (or nullptr on failure)
     */
    [[nodiscard]] Driver* getMountedDriver(Util::String &path);

    Util::HashMap<Util::String, Driver*> mountPoints;
    Util::HashMap<Util::String, MountInformation> mountInformation;
    Util::Async::ReentrantSpinlock lock;
};

}

#endif
