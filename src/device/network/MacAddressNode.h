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

#ifndef HHUOS_MACADDRESSNODE_H
#define HHUOS_MACADDRESSNODE_H

#include "filesystem/memory/StringNode.h"
#include "lib/util/memory/String.h"

namespace Device {
namespace Network {
class NetworkDevice;
}  // namespace Network
}  // namespace Device

namespace Device::Network {

class MacAddressNode : public Filesystem::Memory::StringNode {

public:
    /**
     * Default Constructor.
     */
    explicit MacAddressNode(const NetworkDevice &device);

    /**
     * Copy Constructor.
     */
    MacAddressNode(const MacAddressNode &other) = delete;

    /**
     * Assignment operator.
     */
    MacAddressNode &operator=(const MacAddressNode &other) = delete;

    /**
     * Destructor.
     */
    ~MacAddressNode() override = default;

    /**
     * Overriding function from StringNode.
     */
    Util::Memory::String getString() override;

private:

    const NetworkDevice &device;
};

}

#endif
