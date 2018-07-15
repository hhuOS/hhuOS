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

#include <devices/Pci.h>
#include "PciNode.h"

PciNode::PciNode() : VirtualNode("pci", FsNode::REGULAR_FILE) {

}

uint64_t PciNode::getLength() {

    if (cache.isEmpty()) {
        cacheDeviceList();
    }

    return cache.length();
}

uint64_t PciNode::readData(char *buf, uint64_t pos, uint64_t numBytes) {

    if (cache.isEmpty()) {
        cacheDeviceList();
    }

    uint32_t length = cache.length();

    if (pos + numBytes > length) {
        numBytes = (uint32_t) (length - pos);
    }

    memcpy(buf, (char*) cache + pos, numBytes);

    return numBytes;
}

uint64_t PciNode::writeData(char *buf, uint64_t pos, uint64_t numBytes) {
    return 0;
}

void PciNode::cacheDeviceList() {

    for (auto device : Pci::getDevices()) {
        cache += String::valueOf(device.vendorId, 16) + String(":") + String::valueOf(device.deviceId, 16) + String("\n");
    }
}
