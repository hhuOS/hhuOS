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

#ifndef HHUOS_TRANSMITCONTROL_H
#define HHUOS_TRANSMITCONTROL_H

#include <cstdint>
#include <devices/cpu/Cpu.h>

/**
 * This is an interface for handling basic configurations of the transmit.
 * To set up controls for transmit the register transmit control (TCTL),
 * which is mapped to MMIO-space, is used.
 *
 * "TCTL (00400h;R/W)
 *
 *  This register controls all transmit functions for the Ethernet controller.",
 *  [quote 13.4.33 Transmit Control Register]
 *
 *
 *  31     26 25      22 21  12 11 4 3         0
 * *---------*----------*------*----*-----------*
 * |Reserved | CNTL Bits| COLD | CT | CNTL Bits |
 * *---------*----------*------*----*-----------*
 *
 * Descriptions of the bits are taken from the manual.
 * [see Table 13-76. TCTL Register Bit Description]
 */
class TransmitControl {
public:
    virtual ~TransmitControl() = default;

    TransmitControl(TransmitControl const &) = delete;
    TransmitControl &operator=(TransmitControl const &) = delete;

    /**
     * Enables transmitting.
     * Bit 1 in register.
     * @param enable If true, the transmitter is enabled. Otherwise
     * transmission stops after sending any in progress packets.
     */
    virtual void enableTransmit(bool enable) = 0;

    /**
     * Enables short package padding.
     * Bit 3 in register.
     * @param enable If true, packets smaller than 64 bytes are padded to this size.
     * Otherwise the minimum packet size will be 32 bytes.
     */
    virtual void padShortPackets(bool enable) = 0;

    /**
     * Writes selected bits to a given register.
     */
    virtual void manage() = 0;

    /**
     * This methods are not used for Intel 82541IP. For usage
     * please refer to the manual.
     */

    virtual void collisionThreshold(uint8_t value) = 0;
    virtual void collisionDistance(uint16_t value) = 0;
    virtual void softwareXOffTransmission(bool enable) = 0;
    virtual void retransmitOnLateCollision(bool enable) = 0;
    virtual void noRetransmitOnUnderrun(bool enable) = 0;

protected:
    TransmitControl() = default;
};


#endif //HHUOS_TRANSMITCONTROL_H
