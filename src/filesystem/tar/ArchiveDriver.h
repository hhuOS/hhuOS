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

#ifndef HHUOS_ARCHIVEDRIVER_H
#define HHUOS_ARCHIVEDRIVER_H

#include <filesystem/core/Driver.h>
#include <lib/util/file/tar/Archive.h>

namespace Filesystem::Tar {

class ArchiveDriver : public Driver {

public:
    /**
     * Constructor.
     *
     * @param archive The tar archive to use.
     */
    explicit ArchiveDriver(Util::File::Tar::Archive &archive);

    /**
     * Destructor.
     */
    ~ArchiveDriver() override = default;

    /**
     * Overriding virtual function from Driver.
     */
    Util::Memory::String getClassName() override;

    /**
     * Overriding virtual function from Driver.
     */
    bool mount() override;

    /**
     * Overriding virtual function from Driver.
     */
    Node* getNode(const Util::Memory::String &path) override;

    /**
     * Overriding virtual function from Driver.
     */
    bool createNode(const Util::Memory::String &path) override;

    /**
     * Overriding virtual function from Driver.
     */
    bool deleteNode(const Util::Memory::String &path) override;

private:

    Util::File::Tar::Archive &archive ;
    Util::Data::Array<Util::File::Tar::Archive::Header*> fileHeaders{};

    static const constexpr char *CLASS_NAME = "Filesystem::Tar::ArchiveDriver";

};

}

#endif
