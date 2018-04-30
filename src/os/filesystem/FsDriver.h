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
    virtual bool createFs(StorageDevice *device) = 0;

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