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

#ifndef HHUOS_SMBIOSVERSIONNODE_H
#define HHUOS_SMBIOSVERSIONNODE_H

#include "filesystem/memory/StringNode.h"
#include "lib/util/base/String.h"

namespace Filesystem::SmBios {

class SmBiosVersionNode : public Memory::StringNode {

public:
    /**
     * Default Constructor.
     */
    SmBiosVersionNode();

    /**
     * Copy Constructor.
     */
    SmBiosVersionNode(const SmBiosVersionNode &other) = delete;

    /**
     * Assignment operator.
     */
    SmBiosVersionNode &operator=(const SmBiosVersionNode &other) = delete;

    /**
     * Destructor.
     */
    ~SmBiosVersionNode() override = default;

    /**
     * Overriding function from StringNode.
     */
    Util::String getString() override;

private:

    const Util::String versionString;
};

}

#endif
