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

#ifndef HHUOS_ACPITABLENODE_H
#define HHUOS_ACPITABLENODE_H

#include "filesystem/memory/BufferNode.h"
#include "lib/util/hardware/Acpi.h"

namespace Filesystem::Acpi {

class AcpiTableNode : public Memory::BufferNode {

public:
    /**
     * Constructor.
     */
    AcpiTableNode(const Util::Hardware::Acpi::SdtHeader &table);

    /**
     * Copy Constructor.
     */
    AcpiTableNode(const AcpiTableNode &other) = delete;

    /**
     * Assignment operator.
     */
    AcpiTableNode& operator =(const AcpiTableNode &other) = delete;

    /**
     * Destructor.
     */
    ~AcpiTableNode() override = default;
};

}

#endif
