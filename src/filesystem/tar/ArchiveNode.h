/*
 * Copyright (C) 2018-2021 Heinrich-Heine-Universitaet Duesseldorf,
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

#ifndef HHUOS_ARCHIVENODE_H
#define HHUOS_ARCHIVENODE_H

#include <lib/util/file/tar/Archive.h>
#include <lib/util/memory/Address.h>
#include <lib/util/file/Node.h>

namespace Filesystem::Tar {

class ArchiveNode : public Util::File::Node {

public:
    /**
     * File Constructor.
     */
    ArchiveNode(Util::File::Tar::Archive &archive, Util::File::Tar::Archive::Header fileHeader);

    /**
     * Directory Constructor.
     */
    ArchiveNode(Util::File::Tar::Archive &archive, const Util::Memory::String &path);

    /**
     * Copy-constructor.
     */
    ArchiveNode(const ArchiveNode &copy) = delete;

    /**
     * Destructor.
     */
    ~ArchiveNode() override = default;

    /**
     * Overriding function from Node.
     */
    Util::Memory::String getName() override;

    /**
     * Overriding function from Node.
     */
    Util::File::Type getFileType() override;

    /**
     * Overriding function from Node.
     */
    uint64_t getLength() override;

    /**
     * Overriding function from Node.
     */
    Util::Data::Array<Util::Memory::String> getChildren() override;

    /**
     * Overriding function from Node.
     */
    uint64_t readData(uint8_t *targetBuffer, uint64_t pos, uint64_t numBytes) override;

    /**
     * Overriding function from Node.
     */
    uint64_t writeData(uint8_t *sourceBuffer, uint64_t pos, uint64_t numBytes) override;

private:

    uint32_t size = 0;
    Util::File::Type type;
    Util::Memory::Address<uint32_t> dataAddress;
    Util::Memory::String name;
    Util::Data::ArrayList<Util::Memory::String> children;

};

}

#endif
