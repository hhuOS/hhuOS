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


#ifndef HHUOS_TARARCHIVEDRIVER_H
#define HHUOS_TARARCHIVEDRIVER_H

#include <filesystem/FsDriver.h>
#include <lib/file/tar/Archive.h>

class TarArchiveDriver : public FsDriver {

private:

    Tar::Archive *archive = nullptr;

    Util::Array<Tar::Header*> fileHeaders{};

public:

    FS_DRIVER_IMPLEMENT_CLONE(TarArchiveDriver);

    /**
     * Constructor.
     */
    TarArchiveDriver() = default;

    explicit TarArchiveDriver(Tar::Archive *archive);

    /**
     * Destructor.
     */
    ~TarArchiveDriver() override;

    /**
     * Overriding virtual function from FsDriver.
     */
    bool createFs(StorageDevice *device) override;

    /**
     * Overriding virtual function from FsDriver.
     */
    bool mount(StorageDevice *device) override;

    /**
     * Overriding virtual function from FsDriver.
     */
    FsNode *getNode(const String &path) override;

    /**
     * Overriding virtual function from FsDriver.
     */
    bool createNode(const String &path, uint8_t fileType) override;

    /**
     * Overriding virtual function from FsDriver.
     */
    bool deleteNode(const String &path) override;
};

#endif
