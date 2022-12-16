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

#ifndef HHUOS_PROCESSDRIVER_H
#define HHUOS_PROCESSDRIVER_H

#include "filesystem/core/VirtualDriver.h"
#include "lib/util/file/Type.h"
#include "lib/util/memory/String.h"

namespace Filesystem {
class Node;
}  // namespace Filesystem

namespace Filesystem::Process {

class ProcessDriver : public VirtualDriver {

public:
    /**
     * Default Constructor.
     */
    ProcessDriver() = default;

    /**
     * Copy Constructor.
     */
    ProcessDriver(const ProcessDriver &other) = delete;

    /**
     * Assignment operator.
     */
    ProcessDriver &operator=(const ProcessDriver &other) = delete;

    /**
     * Destructor.
     */
    ~ProcessDriver() override = default;

    /**
     * Overriding virtual function from VirtualDriver.
     */
    Node* getNode(const Util::Memory::String &path) override;

    /**
     * Overriding virtual function from VirtualDriver.
     */
    bool createNode(const Util::Memory::String &path, Util::File::Type type) override;

    /**
     * Overriding virtual function from VirtualDriver.
     */
    bool deleteNode(const Util::Memory::String &path) override;
};

}

#endif
