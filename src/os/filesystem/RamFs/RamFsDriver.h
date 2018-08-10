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

    static const constexpr char *NAME = "ramfs";

protected:

    VirtualNode *rootNode = nullptr;

public:

    FS_DRIVER_IMPLEMENT_CLONE(RamFsDriver);

    /**
     * Constructor.
     */
    RamFsDriver() = default;

    /**
     * Destructor.
     */
    ~RamFsDriver() override;

    /**
     * Overriding function from FsDriver.
     */
    String getName() override;

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
