/*
 * Copyright (C) 2018-2022 Heinrich-Heine-Universitaet Duesseldorf,
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

#ifndef HHUOS_NULLNODE_H
#define HHUOS_NULLNODE_H

#include <cstdint>

#include "MemoryNode.h"
#include "lib/util/file/Type.h"
#include "lib/util/memory/String.h"

namespace Filesystem::Memory {

class NullNode : public MemoryNode {

public:
    /**
     * Constructor.
     */
    explicit NullNode(const Util::Memory::String &name = "null");

    /**
     * Copy Constructor.
     */
    NullNode(const NullNode &copy) = delete;

    /**
     * Assignment operator.
     */
    NullNode& operator=(const NullNode &other) = delete;

    /**
     * Destructor.
     */
    ~NullNode() override = default;

    /**
     * Overriding function from Node.
     */
    Util::File::Type getFileType() override;

    /**
     * Overriding function from Node.
     */
    uint64_t writeData(const uint8_t *sourceBuffer, uint64_t pos, uint64_t numBytes) override;
};

}

#endif
