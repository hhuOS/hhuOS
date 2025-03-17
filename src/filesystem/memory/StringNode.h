/*
 * Copyright (C) 2018-2025 Heinrich-Heine-Universitaet Duesseldorf,
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

#ifndef HHUOS_STRINGNODE_H
#define HHUOS_STRINGNODE_H

#include <stdint.h>

#include "MemoryNode.h"
#include "lib/util/base/String.h"

namespace Filesystem::Memory {

class StringNode : public MemoryNode {

public:
    /**
     * Constructor.
     */
    explicit StringNode(const Util::String &name);

    /**
     * Copy Constructor.
     */
    StringNode(const StringNode &other) = delete;

    /**
     * Assignment operator.
     */
    StringNode &operator=(const StringNode &other) = delete;

    /**
     * Destructor.
     */
    ~StringNode() override = default;

    /**
     * Overriding function from MemoryNode.
     */
    uint64_t getLength() override;

    /**
     * Overriding function from MemoryNode.
     */
    uint64_t readData(uint8_t *targetBuffer, uint64_t pos, uint64_t numBytes) override;

protected:

    virtual Util::String getString() = 0;
};

}

#endif
