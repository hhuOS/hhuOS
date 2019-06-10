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

#ifndef HHUOS_MACADDRESSNODE_H
#define HHUOS_MACADDRESSNODE_H

#include "filesystem/ram/VirtualNode.h"
/**
 * This class implements the class VirtualNode.
 *
 * Once created, the MAC-address can be called from within the
 * shell. The value cannot be changed during runtime.
 */
class MacAddressNode final : public VirtualNode {
private:
    /**
     * String representation of the MAC-address
     */
    String macAddress;

public:
    /**
     * Constructor.
     * @param macAddress The MAC-address from the card who calls.
     */
    explicit MacAddressNode(String macAddress);

    MacAddressNode(const MacAddressNode &copy) = delete;
    ~MacAddressNode() override = default;

    /**
     * Inherited methods from VirtualNode.
     * This methods are meant to be overridden and
     * implemented by this class.
     */

    uint64_t getLength() final;
    uint64_t readData(char *buf, uint64_t pos, uint64_t numBytes) final;
    uint64_t writeData(char *buf, uint64_t pos, uint64_t length) final;
};


#endif //HHUOS_MACADDRESSNODE_H
