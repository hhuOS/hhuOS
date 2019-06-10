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

#ifndef HHUOS_RECEIVECONTROL_H
#define HHUOS_RECEIVECONTROL_H

#include <cstdint>
/**
 * This is an interface for handling basic configurations of the receive.
 * To set up controls for receive the register receive control (RCTL),
 * which is mapped to MMIO-space, is used.
 *
 * "RCTL (00100h; R/W)
 *
 * This register controls all Ethernet controller receiver functions.",
 * [quote 13.4.22 Receive Control Register]
 *
 *  31      27 26                   0
 * *----------*----------------------*
 * | Reserved | Receive Control Bits |
 * *----------*----------------------*
 *
 * Descriptions of the bits are taken from the manual.
 * [see Table 13-67. RCTL Register Bit Description]
 */
class ReceiveControl {
public:
    virtual ~ReceiveControl() = default;

    ReceiveControl(ReceiveControl const &) = delete;
    ReceiveControl &operator=(ReceiveControl const &) = delete;
    /**
     * Enables the receiver.
     * Sets Bit 1 in register.
     * @param enable If true, the receiver is enabled. Otherwise receipt of
     * any in-progress packets will be done and reception stopped.
     */
    virtual void receiver(bool enable) = 0;

    /**
     * Enables storing bad packets.
     * Bad packets are defined as those who have CRC error,
     * symbol error, sequence error, length error, alignment error, short
     * packets or where carrier extension or RX_ERR errors that pass the
     * filter function in host memory.
     * Sets Bit 2 in register.
     * @param enable If true, bad packets will be stored. Otherwise not.
     */
    virtual void storeBadPackets(bool enable) = 0;

    /**
     * Enables unicast promiscuous mode.
     * Sets Bit 3 in register.
     * @param enable If true, unicast packets are not filtered. Otherwise
     * they will be accepted or rejected, "based on received packet destination
     * address match with 1 of 16 stored addresses.",
     * [quote Table 13-67. RCTL Register Bit Description]
     */
    virtual void unicastPromiscuous(bool enable) = 0;

    /**
     * Enables multicast promiscuous mode.
     * 0b = Disabled.
     * 1b = Enabled.
     * When set, passes without filtering out all received multicast packets.
     * Otherwise, the Ethernet controller accepts or rejects a multicast
     * packet based on its 4096-bit vector multicast filtering table.
     * Sets Bit 4 in register.
     * [see Table 13-67. RCTL Register Bit Description]
     * @param enable If true, multicast packets are not filtered. Otherwise
     * they will be accepted or rejected, "based on its 4096-bit vector multicast
     * filtering table.",
     * [quote Table 13-67. RCTL Register Bit Description]
     */
    virtual void multicastPromiscuous(bool enable) = 0;

    /**
     * Enables long packet reception.
     * Sets Bit 5 in register.
     * [see Table 13-67. RCTL Register Bit Description]
     * @param enable If true, packets up to 16384 can be accepted. Otherwise
     * the controller discards packets longer than 1522 bytes.
     */
    virtual void longPacketReception(bool enable) = 0;

    /**
     * Defines the descriptor minimum threshold size.
     * "The corresponding interrupt ICR.RXDMT0 is set each time the
     *  fractional number of free descriptors becomes equal to RDMTS.
     *  The following table lists which fractional values correspond to
     *  RDMTS values. The size of the total receiver circular descriptor
     *  buffer is set by RDLEN. See Section 13.4.27 for details regarding
     *  RDLEN.
     *  00b = Free Buffer threshold is set to 1/2 of RDLEN.
     *  01b = Free Buffer threshold is set to 1/4 of RDLEN.
     *  10b = Free Buffer threshold is set to 1/8 of RDLEN.
     *  11b = Reserved.",
     *  [quote Table 13-67. RCTL Register Bit Description]
     *
     * Sets Bits 9:8 in register.
     * @param value Fraction size to be set. The lower 2 bits are relevant.
     */
    virtual void descriptorMinimumThresholdSize(uint8_t value) = 0;

    /**
     * Enables accepting all broadcast packets.
     * Sets Bit 15 in register.
     * @param enable If true, broadcast packets will be accepted and not be
     * filtered out. Otherwise they will be checked against filters.
     */
    virtual void broadCastAcceptMode(bool enable) = 0;

    /**
     * Sets the size of the receive buffers.
     * If the method bufferSizeExtension disables its bit
     * the mapping is
     * 00b = 2048 Bytes.
     * 01b = 1024 Bytes.
     * 10b = 512 Bytes.
     * 1b1 = 256 Bytes.
     * Otherwise this values will be multiplied by 16.
     * Sets Bits 17:16 in register.
     * @param value Size of the buffer. The lower 2 bits are relevant.
     */
    virtual void bufferSize(uint16_t value) = 0;

    /**
     * Enables extending of the receive buffer sizes.
     * Sets Bit 25 in register.
     * @param enable If true, extended buffers are used. Otherwise not.
     */
    virtual void bufferSizeExtension(bool enable) = 0;

    /**
     * Enables stripping off the ethernet CRC field.
     * "Controls whether the hardware strips the Ethernet CRC from the
     *  received packet. This stripping occurs prior to any checksum
     *  calculations. The stripped CRC is not transferred to host memory
     *  and is not included in the length reported in the descriptor."
     *  Sets Bit 26 in register.",
     *  [quote Table 13-67. RCTL Register Bit Description]
     * @param enable If true, the CRC of the ethernet frame will be stripped.
     * Otherwise not.
     */
    virtual void stripEthernetCrc(bool enable) = 0;

    /**
     * Writes selected bits to a given register.
     */
    virtual void manage() = 0;

    /**
     * This methods are not used for Intel 82541IP. For usage
     * please refer to the manual.
     */

    virtual void vlanFilter(bool enable) = 0;
    virtual void canonicalFormIndicator(bool enable) = 0;
    virtual void canonicalFormIndicatorBitValue(bool enable) = 0;
    virtual void discardPauseFrames(bool enable) = 0;
    virtual void passMacControlFrames(bool enable) = 0;
    virtual void loopbackMode(uint8_t value) = 0;
    virtual void multicastOffset(uint8_t value) = 0;

protected:
    ReceiveControl() = default;
};


#endif //HHUOS_RECEIVECONTROL_H
