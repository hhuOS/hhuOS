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

#ifndef HHUOS_FATNODE_H
#define HHUOS_FATNODE_H

#include <stdint.h>

#include "filesystem/fat/ff/source/ff.h"
#include "filesystem/Node.h"
#include "lib/util/base/String.h"

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
    ~FatNode() override = default;

    static FatNode* open(const Util::String &path);

    /**
     * Overriding function from Node.
     */
    Util::String getName() override;

    /**
     * Overriding function from Node.
     */
    uint64_t getLength() override;

protected:
    /**
     * Constructor.
     */
    explicit FatNode(const Util::String &path);

private:

    FILINFO stat();

    Util::String path;
};

}

#endif
