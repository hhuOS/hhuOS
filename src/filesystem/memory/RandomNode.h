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

#ifndef HHUOS_RANDOMNODE_H
#define HHUOS_RANDOMNODE_H

#include <stdint.h>

#include "MemoryNode.h"
#include "lib/util/base/String.h"
#include "lib/util/io/file/File.h"

namespace Filesystem::Memory {

class RandomNode : public MemoryNode {

public:
    /**
     * Constructor.
     */
    explicit RandomNode(const Util::String &name = "random");

    /**
     * Copy Constructor.
     */
    RandomNode(const RandomNode &copy) = delete;

    /**
     * Assignment operator.
     */
    RandomNode &operator=(const RandomNode &other) = delete;

    /**
     * Destructor.
     */
    ~RandomNode() override = default;

    /**
     * Overriding function from Node.
     */
    Util::Io::File::Type getType() override;

    /**
     * Overriding function from Node.
     */
    uint64_t readData(uint8_t *targetBuffer, uint64_t pos, uint64_t numBytes) override;

    bool isReadyToRead() override;
};

}

#endif
