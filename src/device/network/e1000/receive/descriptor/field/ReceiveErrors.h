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

#ifndef HHUOS_RECEIVEERRORS_H
#define HHUOS_RECEIVEERRORS_H

#include <cstdint>
/**
 * This is an interface for accessing the errors field of a receive descriptor.
 *
 * "Most error information appears only when the Store Bad Packets bit (RCTL.SBP)
 *  is set and a bad packet is received. [...] The error bits are valid only when
 *  the EOP and DD bits are set in the descriptor status field.",
 *  [quote 3.2.3.2 Receive Descriptor Errors Field]
 *
 * Descriptions of the bits are taken from the manual.
 * [see Table 3-3. Receive Errors (RDESC.ERRORS) Layout]
 */
class ReceiveErrors {
public:
    virtual ~ReceiveErrors() = default;

    ReceiveErrors(ReceiveErrors const &) = delete;
    ReceiveErrors &operator=(ReceiveErrors const &) = delete;

    /**
     * "CRC Error or Alignment Error
     *  CRC errors and alignment errors are both indicated via the CE bit. Software may
     *  distinguish between these errors by monitoring the respective statistics registers.",
     *  [quote Table 3-3. Receive Errors (RDESC.ERRORS) Layout]
     *
     * Bit 0 in field.
     */
    virtual bool crcOrAlignmentError() = 0;

    /**
     * "TCP/UDP Checksum Error
     *  When set, indicates that TCP/UDP checksum error is detected in the received
     *  packet.
     *  Valid only when the TCP/UDP checksum is performed on the receive packet as
     *  indicated via TCPCS bit in RDESC.STATUS field.
     *  If receive TCP/UDP checksum offloading is disabled (RXCSUM.TUOFL), the TCPE
     *  bit is set to 0b.
     *  It has no effect on the packet filtering mechanism.
     *  Reads as 0b.",
     *  [quote Table 3-3. Receive Errors (RDESC.ERRORS) Layout]
     *
     * Bit 5 in field.
     */
    virtual bool tcpUdpChecksumError() = 0;

    /**
     * "IP Checksum Error
     *  When set, indicates that IP checksum error is detected in the received packet. Valid
     *  only when the IP checksum is performed on the receive packet as indicated via the
     *  IPCS bit in the RDESC.STATUS field.
     *  If receive IP checksum offloading is disabled (RXCSUM.IPOFL), the IPE bit is set to
     *  0b. It has no effect on the packet filtering mechanism.
     *  Reads as 0b.",
     *  [quote Table 3-3. Receive Errors (RDESC.ERRORS) Layout]
     *
     * Bit 6 in field.
     */
    virtual bool ipChecksumError() = 0;

    /**
     * Verifies if there has any error occurred.
     * @return True if there was any error, otherwise false.
     */
    virtual bool hasErrors() = 0;

    /**
     * Retrieve the whole error-Byte from the memory.
     * @return Error-Byte of the according descriptor.
     */
    virtual uint8_t loadFromMemory() = 0;

    /**
     * This methods are not used for Intel 82541IP. For usage
     * please refer to the manual.
     */

    virtual bool rxDataError() = 0;
    virtual bool symbolError() = 0;
    virtual bool sequenceError() = 0;
    virtual bool carrierExtensionError() = 0;

protected:
    ReceiveErrors() = default;
};


#endif //HHUOS_RECEIVEERRORS_H
