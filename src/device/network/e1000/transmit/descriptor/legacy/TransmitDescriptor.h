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

#ifndef HHUOS_TRANSMITDESCRIPTOR_H
#define HHUOS_TRANSMITDESCRIPTOR_H

#include <cstdint>
#include "kernel/log/Logger.h"

/**
 * This is an interface for handling the transmit descriptor.
 *
 *      63     48 47 40 39 36 35 32 31 24 23 16 15     0
 *     *---------*-----*-----*-----*-----*-----*--------*
 * 0x0 |             Buffer Address [63:0]              |
 *     *---------*-----*-----*-----*-----*-----*--------*
 * 0x8 | Special | CSS | RSV | STA | CMD | CSO | Length |
 *     *---------*-----*-----*-----*-----*-----*--------*
 *
 * "Notes:
 *   Even though CSO and CSS are in units of bytes, the checksum calculation typically works on
 *   16-bit words. Hardware does not enforce even byte alignment.
 *
 *   Hardware does not add the 802.1Q EtherType or the VLAN field following the 802.1Q
 *   EtherType to the checksum. So for VLAN packets, software can compute the values to back
 *   out only on the encapsulated packet rather than on the added fields.
 *
 *   Although the Ethernet controller can be programmed to calculate and insert TCP checksum
 *   using the legacy descriptor format as described above, it is recommended that software use the
 *   newer TCP/IP Context Transmit Descriptor Format. This newer descriptor format allows the
 *   hardware to calculate both the IP and TCP checksums for outgoing packets. See Section 3.3.5
 *   for more information about how the new descriptor format can be used to accomplish this task."
 *   [quote 3.3.3 Legacy Transmit Descriptor Format]
 *
 * Descriptions of the bits are taken from the manual.
 * [see Table 3-9. Transmit Descriptor Legacy Descriptions]
 */
class TransmitDescriptor {
public:
    virtual ~TransmitDescriptor() = default;

    TransmitDescriptor(TransmitDescriptor const &) = delete;
    TransmitDescriptor &operator=(TransmitDescriptor const &) = delete;

    /**
     * Writes the physical address of the packet to send.
     * Bits 0-63:0 in layout.
     * @param address The physical address of the packet to send.
     * Null addresses indicate that there is no packet to send.
     */
    virtual void writeAddress(uint64_t address) = 0;

    /**
     * Writes the length of the packet.
     * Bits 1-15:0 in layout.
     * @param length The length of the packet.
     */
    virtual void writeLength(uint16_t length) = 0;

    /**
     * Writes the command field to it's according descriptor.
     */
    virtual void writeCommand() = 0;

    /**
     * Checks if hardware is done with the descriptor.
     * @return True, if hardware is done. Otherwise false.
     */
    virtual bool isDone() = 0;

    /**
     * This methods are not used for Intel 82541IP. For usage
     * please refer to the manual.
     */

    virtual void writeChecksumOffset(uint8_t offset) = 0;
    virtual void writeChecksumStartField(uint8_t start) = 0;
    virtual void writeSpecial(uint16_t value) = 0;

protected:
    TransmitDescriptor() = default;
};


#endif //HHUOS_TRANSMITDESCRIPTOR_H
