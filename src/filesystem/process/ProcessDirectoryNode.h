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

#ifndef HHUOS_PROCESSDIRECTORYNODE_H
#define HHUOS_PROCESSDIRECTORYNODE_H

#include <stdint.h>

#include "filesystem/Node.h"
#include "lib/util/base/String.h"
#include "lib/util/io/file/File.h"

namespace Filesystem::Process {

class ProcessDirectoryNode : public Node {

public:
    /**
     * Default Constructor.
     */
    explicit ProcessDirectoryNode(uint32_t processId);

    /**
     * Copy Constructor.
     */
    ProcessDirectoryNode(const ProcessDirectoryNode &other) = delete;

    /**
     * Assignment operator.
     */
    ProcessDirectoryNode &operator=(const ProcessDirectoryNode &other) = delete;

    /**
     * Destructor.
     */
    ~ProcessDirectoryNode() override = default;

    /**
     * Overriding function from Node.
     */
    Util::String getName() override;

    /**
     * Overriding function from Node.
     */
    Util::Io::File::Type getType() override;

    /**
     * Overriding function from Node.
     */
    uint64_t getLength() override;

    /**
     * Overriding function from Node.
     */
    Util::Array<Util::String> getChildren() override;

    /**
     * Overriding function from Node.
     */
    uint64_t readData(uint8_t *targetBuffer, uint64_t pos, uint64_t numBytes) override;

    /**
     * Overriding function from Node.
     */
    uint64_t writeData(const uint8_t *sourceBuffer, uint64_t pos, uint64_t numBytes) override;

private:

    Util::String name;
};

}

#endif
