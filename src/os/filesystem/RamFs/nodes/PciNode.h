/*
 * Copyright (C) 2018 Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schoettner
 * Heinrich-Heine University
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

#ifndef __PciNode_include__
#define __PciNode_include__

#include "filesystem/RamFs/VirtualNode.h"
#include <cstdint>
#include <lib/String.h>

/**
 * Implementation of VirtualNode, that reads the the IDs of all PCI-devices from the PCI-class.
 *
 * @author Filip Krakowski
 * @date 2018
 */
class PciNode : public VirtualNode {

private:
    String cache;

    void cacheDeviceList();

public:
    /**
     * Constructor
     */
    PciNode();

    /**
     * Copy-constructor.
     */
    PciNode(const PciNode &copy) = delete;

    /**
     * Destructor.
     */
    ~PciNode() override = default;

    /**
     * Overriding function from VirtualNode.
     */
    uint64_t getLength() override;

    /**
     * Overriding function from VirtualNode.
     */
    uint64_t readData(char *buf, uint64_t pos, uint64_t numBytes) override;

    /**
     * Overriding function from VirtualNode.
     */
    uint64_t writeData(char *buf, uint64_t pos, uint64_t numBytes) override;
};

#endif