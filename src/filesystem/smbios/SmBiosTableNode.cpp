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

#include "SmBiosTableNode.h"

#include "lib/util/base/String.h"

namespace Filesystem::SmBios {

uint8_t SmBiosTableNode::typeCounter[256]{};

SmBiosTableNode::SmBiosTableNode(const Util::Hardware::SmBios::TableHeader &tableHeader) :
        Memory::BufferNode(Util::String::format("%u-%u", tableHeader.type, typeCounter[tableHeader.type]++),
                           reinterpret_cast<const uint8_t *>(&tableHeader), tableHeader.calculateFullLength()) {}

};