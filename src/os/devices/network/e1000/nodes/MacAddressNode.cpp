/*
 * Copyright (C) 2018/19 Thiemo Urselmann
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
 *
 * Note:
 * All references marked with [...] refer to the following developers manual.
 * Intel Corporation. PCI/PCI-X Family of Gigabit Ethernet Controllers Software Developer’s Manual.
 * 317453006EN.PDF Revision 4.0. 2009.
 */

#include "MacAddressNode.h"

MacAddressNode::MacAddressNode(String macAddress)
        : VirtualNode("mac", FsNode::REGULAR_FILE), macAddress(macAddress) {
}

uint64_t MacAddressNode::getLength() {
    return macAddress.length();
}

uint64_t MacAddressNode::readData(char *buf, uint64_t pos, uint64_t numBytes) {
    uint64_t length = macAddress.length();

    if (pos + numBytes > length) {
        numBytes = (uint32_t) (length - pos);
    }

    //numBytes will not exceed a 32-bit value, since MAC-addresses
    //usually have 6 bytes.
    memcpy(buf, (char*) macAddress + pos,  (uint32_t) numBytes);

    return numBytes;
}

uint64_t MacAddressNode::writeData(char *buf, uint64_t pos, uint64_t length) {
    Cpu::throwException(Cpu::Exception::NULLPOINTER, "Method not supported");
    return 0;
}

