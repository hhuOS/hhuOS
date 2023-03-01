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

#ifndef HHUOS_ARCHIVEDRIVER_H
#define HHUOS_ARCHIVEDRIVER_H

#include "filesystem/core/VirtualDriver.h"
#include "lib/util/io/file/tar/Archive.h"
#include "lib/util/collection/Array.h"
#include "lib/util/base/String.h"
#include "lib/util/io/file/File.h"

namespace Filesystem {
class Node;
}  // namespace Filesystem

namespace Filesystem::Tar {

class ArchiveDriver : public VirtualDriver {

public:
    /**
     * Constructor.
     *
     * @param archive The tar archive to use.
     */
    explicit ArchiveDriver(Util::Io::Tar::Archive &archive);

    /**
     * Copy Constructor.
     */
    ArchiveDriver(const ArchiveDriver &copy) = delete;

    /**
     * Assignment operator.
     */
    ArchiveDriver& operator=(const ArchiveDriver &other) = delete;

    /**
     * Destructor.
     */
    ~ArchiveDriver() override = default;

    /**
     * Overriding virtual function from VirtualDriver.
     */
    Node* getNode(const Util::String &path) override;

    /**
     * Overriding virtual function from VirtualDriver.
     */
    bool createNode(const Util::String &path, Util::Io::File::Type type) override;

    /**
     * Overriding virtual function from VirtualDriver.
     */
    bool deleteNode(const Util::String &path) override;

private:

    Util::Io::Tar::Archive &archive;
    Util::Array<Util::Io::Tar::Archive::Header> fileHeaders{};

};

}

#endif
