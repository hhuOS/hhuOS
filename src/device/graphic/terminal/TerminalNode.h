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

#ifndef HHUOS_TERMINALNODE_H
#define HHUOS_TERMINALNODE_H

#include "filesystem/memory/MemoryNode.h"
#include "device/graphic/terminal/Terminal.h"

namespace Device::Graphic {

class TerminalNode : public Filesystem::Memory::MemoryNode {

public:
    /**
     * Default Constructor.
     */
    TerminalNode(const Util::Memory::String &name, Device::Graphic::Terminal *terminal);

    /**
     * Copy Constructor.
     */
    TerminalNode(const TerminalNode &other) = delete;

    /**
     * Assignment operator.
     */
    TerminalNode &operator=(const TerminalNode &other) = delete;

    /**
     * Destructor.
     */
    ~TerminalNode() override;

    /**
     * Overriding function from MemoryNode.
     */
    Util::File::Type getFileType() override;

    /**
     * Overriding function from MemoryNode.
     */
    uint64_t writeData(const uint8_t *sourceBuffer, uint64_t pos, uint64_t numBytes) override;

private:

    Device::Graphic::Terminal &terminal;

};

}

#endif