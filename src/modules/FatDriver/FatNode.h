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

#ifndef __FatNode_include__
#define __FatNode_include__

#include "filesystem/FsNode.h"
#include "FatFs.h"
#include <cstdint>

extern "C" {
#include "lib/libc/string.h"
}

/**
 * An implementation of FsNode for the FatFs-library.
 *
 * @author Fabian Ruhland
 * @date 2018
 */
class FatNode : public FsNode {

private:
    union FatObject {
        FIL file;
        DIR dir;
    };

    FatFs *fatInstance;
    FatObject fatObject{};
    FILINFO info{};
    String path{};

    /**
     * Constructor.
     *
     * @param fatInstance Instance of FatDriver
     */
    explicit FatNode(FatFs *fatInstance);

public:
    /**
     * Copy-constructor.
     */
    FatNode(const FatNode &copy) = delete;

    /**
     * Destructor.
     */
    ~FatNode() override;

    /**
     * Open a FatNode.
     * CAUTION: May return nullptr, if the file does not exist.
     *          Always check the return value!
     *
     * @param path The absolute path (inside the FAT-filesystem), that points to the file
     * @param fatInstance The instance of FatDriver, that manages the drive, on which the requested file is located.
     *
     * @return The FatNode (or nulltpr on failure)
     */
    static FatNode *open(const String &path, FatFs *fatInstance);

    /**
     * Overriding function from FsNode.
     */
    String getName() override;

    /**
     * Overriding function from FsNode.
     */
    uint8_t getFileType() override;

    /**
     * Overriding function from FsNode.
     */
    uint64_t getLength() override;

    /**
     * Overriding function from FsNode.
     */
    Util::Array<String> getChildren() override;

    /**
     * Overriding function from FsNode.
     */
    uint64_t readData(char *buf, uint64_t pos, uint64_t numBytes) override;

    /**
     * Overriding function from FsNode.
     */
    uint64_t writeData(char *buf, uint64_t pos, uint64_t numBytes) override;
};

#endif
