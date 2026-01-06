/*
 * Copyright (C) 2017-2026 Heinrich Heine University Düsseldorf,
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

#ifndef HHUOS_BUFFERNODE_H
#define HHUOS_BUFFERNODE_H

#include <stdint.h>

#include "lib/util/base/String.h"
#include "MemoryNode.h"

namespace Filesystem::Memory {

class BufferNode : public MemoryNode {

public:
    /**
     * Constructor.
     */
    BufferNode(const Util::String &name, const uint8_t *buffer, uint32_t length);

    /**
     * Copy Constructor.
     */
    BufferNode(const BufferNode &other) = delete;

    /**
     * Assignment operator.
     */
    BufferNode &operator=(const BufferNode &other) = delete;

    /**
     * Destructor.
     */
    ~BufferNode() override = default;

    /**
     * Overriding function from MemoryNode.
     */
    uint64_t getLength() override;

    /**
     * Overriding function from MemoryNode.
     */
    uint64_t readData(uint8_t *targetBuffer, uint64_t pos, uint64_t numBytes) override;

    bool isReadyToRead() override;

private:

    const uint8_t *buffer;
    uint32_t length;
};

}

#endif
