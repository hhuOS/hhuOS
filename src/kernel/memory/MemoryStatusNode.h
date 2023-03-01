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

#ifndef HHUOS_MEMORYSTATUSNODE_H
#define HHUOS_MEMORYSTATUSNODE_H

#include <cstdint>

#include "filesystem/memory/StringNode.h"
#include "lib/util/base/String.h"

namespace Kernel {

class MemoryStatusNode : public Filesystem::Memory::StringNode {

public:
    /**
     * Constructor.
     */
    explicit MemoryStatusNode(const Util::String &name);

    /**
     * Copy Constructor.
     */
    MemoryStatusNode(const MemoryStatusNode &copy) = delete;

    /**
     * Assignment operator.
     */
    MemoryStatusNode& operator=(const MemoryStatusNode &other) = delete;

    /**
     * Destructor.
     */
    ~MemoryStatusNode() override = default;

    /**
     * Overriding function from StringNode.
     */
     Util::String getString() override;

private:

    static Util::String formatMemory(uint32_t value);

    Util::String memoryStatusBuffer;

};

}

#endif
