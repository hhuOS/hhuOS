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

#ifndef HHUOS_FATNODE_H
#define HHUOS_FATNODE_H

#include <cstdint>

#include "filesystem/fat/ff/source/ff.h"
#include "filesystem/core/Node.h"
#include "lib/util/memory/String.h"

namespace Filesystem::Fat {

class FatNode : public Node {

public:
    /**
     * Copy Constructor.
     */
    FatNode(const FatNode &copy) = delete;

    /**
     * Assignment operator.
     */
    FatNode &operator=(const FatNode &other) = delete;

    /**
     * Destructor.
     */
    ~FatNode() override;

    static FatNode* open(const Util::Memory::String &path);

    /**
     * Overriding function from Node.
     */
    Util::Memory::String getName() override;

    /**
     * Overriding function from Node.
     */
    uint64_t getLength() override;

protected:
    /**
     * Constructor.
     */
    explicit FatNode(FILINFO *info);

private:

    FILINFO &info;
};

}

#endif
