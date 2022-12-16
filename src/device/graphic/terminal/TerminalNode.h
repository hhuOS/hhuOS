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

#include <cstdint>

#include "filesystem/memory/StreamNode.h"
#include "lib/util/data/Array.h"
#include "lib/util/memory/String.h"

namespace Util {
namespace Graphic {
class Terminal;
}  // namespace Graphic
}  // namespace Util

namespace Device::Graphic {

class TerminalNode : public Filesystem::Memory::StreamNode {

public:
    /**
     * Default Constructor.
     */
    TerminalNode(const Util::Memory::String &name, Util::Graphic::Terminal *terminal);

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
    ~TerminalNode() override = default;

    bool control(uint32_t request, const Util::Data::Array<uint32_t> &parameters) override;

private:

    Util::Graphic::Terminal *terminal;
};

}

#endif
