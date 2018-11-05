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

#define FS_DRIVER_IMPLEMENT_CLONE(TYPE) FsDriver *clone() const override { return new TYPE(*this); }

#include "FsNode.h"
#include <lib/util/HashMap.h>
#include "devices/storage/devices/StorageDevice.h"

/**
 * An interface between the FileSystem-class and a filesystem-driver.
 * Every filesystem-driver needs to implement the functions,
 * that are specified by this class. The FileSystem-class can then communicate
 * with the respective driver.
 *
 * Implementation of the prototype pattern is based on
 * http://www.cs.sjsu.edu/faculty/pearce/modules/lectures/oop/types/reflection/prototype.htm
 */
class FsDriver {

private:

    /**
     * Contains prototypes for all available filesystem drivers.
     */
    static Util::HashMap<String, FsDriver*> prototypeTable;

public:

    /**
     * Create a copy of this instance.
     *
     * @return A pointer to the copy
     */
    virtual FsDriver *clone() const = 0;

    /**
     * Get the name, under which the driver will be registered and usable for the user.
     */
    virtual String getName() = 0;

    /**
     * Create a new instance of a given subtype of FsDriver.
     * Throws an exception, if the type is unknown.
     *
     * @param type The type
     *
     * @return A pointer to newly created instance
     */
    static FsDriver *createInstance(String type);

    /**
     * Add a new type of FsDriver.
     * Instance of this type can then be create by calling 'FsDriver::createInstance(type)'.
     *
     * @param type The type
     * @param driver Instance, that will be used as a prototype for further instances
     */
    static void registerDriverType(FsDriver *driver);

    /**
     * Remove a type of FsDriver.
     *
     * @param type The type
     */
    static void deregisterDriverType(String type);

public:
    /**
     * Constructor.
     */
    FsDriver() = default;

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