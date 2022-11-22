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

#include "lib/util/stream/ByteArrayOutputStream.h"
#include "MacAddressNode.h"

namespace Device::Network {

MacAddressNode::MacAddressNode(const NetworkDevice &device) : StringNode("mac_address"), device(device) {}

Util::Memory::String MacAddressNode::getString() {
    const auto address = device.getMacAddress().getAddress();
    return Util::Memory::String::format("%02x:%02x:%02x:%02x:%02x:%02x\n", address.buffer[0], address.buffer[1], address.buffer[2], address.buffer[3], address.buffer[4], address.buffer[5]);
}

}