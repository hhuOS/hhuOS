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
 * 317453006EN.PDF Revision 4.0. 2009
 */

#ifndef HHUOS_RECEIVESTATUS_H
#define HHUOS_RECEIVESTATUS_H
/**
 * This is an interface for accessing the status field of a receive descriptor.
 *
 * "Status information indicates whether the descriptor has been used and whether the referenced
 *  buffer is the last one for the packet. [...] If EOP is not set for a descriptor, only
 *  the Address, Length, and DD bits are valid.",
 *  [quote 3.2.3.1 Receive Descriptor Status Field]
 *
 * Descriptions of the bits are taken from the manual.
 * [see Table 3-2. Receive Status (RDESC.STATUS) Layout]
 */
class ReceiveStatus {
public:
    virtual ~ReceiveStatus() = default;

    ReceiveStatus(ReceiveStatus const &) = delete;
    ReceiveStatus &operator=(ReceiveStatus const &) = delete;

    /**
     * "Descriptor Done
     *  Indicates whether hardware is done with the descriptor. When set along with
     *  EOP, the received packet is complete in main memory.",
     *  [quote Table 3-2. Receive Status (RDESC.STATUS) Layout]
     *
     * Bit 0 in field.
     * @return True, if descriptor is done. Otherwise false.
     */
    virtual bool descriptorDone() = 0;

    /**
     * "End of Packet
     *  EOP indicates whether this is the last descriptor for an incoming packet.",
     *  [quote Table 3-2. Receive Status (RDESC.STATUS) Layout]
     *
     * Bit 1 in field.
     * @return True, if this is the last descriptor for the packet. Otherwise false.
     */
    virtual bool endOfPacket() = 0;

    /**
     * Load the whole Status-Byte from from Memory.
     * @return The currently stored Status of the according descriptor.
     */
    virtual uint8_t loadFromMemory() = 0;

    /**
     * Store 0 in the Status-Field.
     */
    virtual void clearInMemory() = 0;

    /**
     * This methods are not used for Intel 82541IP. For usage
     * please refer to the manual.
     */

    virtual bool ignoreChecksumIndication() = 0;
    virtual bool matchesVet() = 0;
    virtual bool tcpChecksumCalculatedOnPacket() = 0;
    virtual bool ipChecksumCalculatedOnPacket() = 0;
    virtual bool passedInExactFilter() = 0;

protected:
    ReceiveStatus() = default;
};


#endif //HHUOS_RECEIVESTATUS_H
