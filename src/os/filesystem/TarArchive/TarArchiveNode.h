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

#ifndef HHUOS_TARARCHIVENODE_H
#define HHUOS_TARARCHIVENODE_H

#include <filesystem/FsNode.h>
#include <lib/file/tar/Archive.h>

class TarArchiveNode : public FsNode {

private:

    FileType fileType;

    String fileName;

    uint32_t fileSize = 0;

    uint8_t *fileBuffer = nullptr;

    Util::ArrayList<String> children;

public:
    /**
     * Constructor.
     */
    TarArchiveNode(Tar::Archive *archive, Tar::Header *fileHeader);

    TarArchiveNode(Tar::Archive *archive, const String &path);

    /**
     * Copy-constructor.
     */
    TarArchiveNode(const TarArchiveNode &copy) = delete;

    /**
     * Destructor.
     */
    ~TarArchiveNode() override = default;

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

private:

    uint32_t calculateFileSize(const uint8_t octalSize[12]);
};

#endif
