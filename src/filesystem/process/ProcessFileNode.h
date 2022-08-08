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

#ifndef HHUOS_PROCESSFILENODE_H
#define HHUOS_PROCESSFILENODE_H

#include "lib/util/memory/String.h"
#include "filesystem/core/Node.h"

namespace Filesystem::Process {

class ProcessFileNode : public Node {

public:
    /**
     * Default Constructor.
     */
    ProcessFileNode(const Util::Memory::String &name, const Util::Memory::String &content);

    /**
     * Copy Constructor.
     */
    ProcessFileNode(const ProcessFileNode &other) = delete;

    /**
     * Assignment operator.
     */
    ProcessFileNode &operator=(const ProcessFileNode &other) = delete;

    /**
     * Destructor.
     */
    ~ProcessFileNode() override = default;

    /**
     * Overriding function from Node.
     */
    Util::Memory::String getName() override;

    /**
     * Overriding function from Node.
     */
    Util::File::Type getFileType() override;

    /**
     * Overriding function from Node.
     */
    uint64_t getLength() override;

    /**
     * Overriding function from Node.
     */
    Util::Data::Array<Util::Memory::String> getChildren() override;

    /**
     * Overriding function from Node.
     */
    uint64_t readData(uint8_t *targetBuffer, uint64_t pos, uint64_t numBytes) override;

    /**
     * Overriding function from Node.
     */
    uint64_t writeData(const uint8_t *sourceBuffer, uint64_t pos, uint64_t numBytes) override;

private:

    Util::Memory::String name;
    Util::Memory::String buffer;
};

}

#endif
