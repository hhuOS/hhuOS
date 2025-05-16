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

#ifndef HHUOS_SMBIOSTABLENODE_H
#define HHUOS_SMBIOSTABLENODE_H

#include <stdint.h>

#include "lib/util/hardware/SmBios.h"
#include "filesystem/memory/BufferNode.h"

namespace Filesystem::SmBios {

class SmBiosTableNode : public Memory::BufferNode {

public:
    /**
     * Constructor.
     */
    explicit SmBiosTableNode(const Util::Hardware::SmBios::TableHeader &tableHeader);

    /**
     * Copy Constructor.
     */
    SmBiosTableNode(const SmBiosTableNode &other) = delete;

    /**
     * Assignment operator.
     */
    SmBiosTableNode& operator=(const SmBiosTableNode &other) = delete;

    /**
     * Destructor.
     */
    ~SmBiosTableNode() override = default;

private:

    static uint8_t typeCounter[256];
};

}

#endif
