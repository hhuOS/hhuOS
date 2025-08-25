/*
 * Copyright (C) 2017-2025 Heinrich Heine University Düsseldorf,
 * Institute of Computer Science, Department Operating Systems
 * Main developers: Christian Gesse <christian.gesse@hhu.de>, Fabian Ruhland <ruhland@hhu.de>
 * Original development team: Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schöttner
 * This project has been supported by several students.
 * A full list of integrated student theses can be found here: https://github.com/hhuOS/hhuOS/wiki/Student-theses
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

#ifndef HHUOS_PIPEDIRECTORYNODE_H
#define HHUOS_PIPEDIRECTORYNODE_H

#include "filesystem/memory/MemoryNode.h"

namespace Filesystem::Process {

class PipeDirectoryNode : public Memory::MemoryNode {

public:
    /**
     * Default Constructor.
     */
    PipeDirectoryNode(uint32_t processId);

    /**
     * Copy Constructor.
     */
    PipeDirectoryNode(const PipeDirectoryNode &other) = delete;

    /**
     * Assignment operator.
     */
    PipeDirectoryNode &operator=(const PipeDirectoryNode &other) = delete;

    /**
     * Destructor.
     */
    ~PipeDirectoryNode() override = default;

    /**
     * Overriding function from Node.
     */
    Util::Io::File::Type getType() override;

    /**
     * Overriding function from Node.
     */
    Util::Array<Util::String> getChildren() override;

private:

    uint32_t processId;
};

}

#endif