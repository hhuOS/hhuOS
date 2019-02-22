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

#ifndef HHUOS_INTERRUPTENABLE_H
#define HHUOS_INTERRUPTENABLE_H

#include <cstdint>

/**
 * This interface provides methods for enabling individual interrupts.
 * To enable interrupts the register Interrupt Mask Set (IMS), which is mapped to MMIO-space, is used.
 *
 * "IMS (000D0h; R/W)
 *
 *  An interrupt is enabled if its corresponding mask bit is set to 1b, and disabled if its corresponding
 *  mask bit is set to 0b. A PCI interrupt is generated each time one of the bits in this register is set and
 *  the corresponding interrupt condition occurs. The occurrence of an interrupt condition is reflected
 *  by having a bit set in the Interrupt Cause Read Register (see Section 13.4.17).
 *
 *  A particular interrupt can be enabled by writing a 1b to the corresponding mask bit in this register.
 *  Any bits written with a 0b are unchanged. As a result, if software desires to disable a particular
 *  interrupt condition that had been previously enabled, it must write to the Interrupt Mask Clear
 *  Register (see Section 13.4.21) rather than writing a 0b to a bit in this register.
 *
 *  Reading this register returns bits that have an interrupt mask set.",
 *  [quote 13.4.20 Interrupt Mask Set/Read Register]
 *
 *  31      17 16       0
 * *----------*----------*
 * | Reserved | IMS Bits |
 * *----------*----------*
 *
 * Descriptions of the bits are taken from the manual.
 * [see Table 13-65. IMS Register Bit Description]
 *
 * Note:
 * There is a special situation for the 82547GI/EI, described by the manual.
 */
class InterruptEnable {
public:
    virtual ~InterruptEnable() = default;

    InterruptEnable(InterruptEnable const &) = delete;
    InterruptEnable &operator=(InterruptEnable const &) = delete;

    /**
     * "Sets mask for Link Status Change.",
     * [quote Table 13-65. IMS Register Bit Description]
     * Bit 2 in register.
     */
    virtual void linkStatusChange() = 0;

    /**
     * "Sets mask for Receive Descriptor Minimum Threshold hit."
     * [quote Table 13-65. IMS Register Bit Description]
     * Bit 4 in register.
     */
    virtual void receiveDescriptorMinimumThresholdHit() = 0;

    /**
     * "Sets mask for on Receiver FIFO Overrun.",
     * [quote Table 13-65. IMS Register Bit Description]
     * Bit 6 in register.
     */
    virtual void receiverFifoOverrun() = 0;

    /**
     * "Sets mask for Receiver Timer Interrupt.",
     * [quote Table 13-65. IMS Register Bit Description]
     * Bit 7 in register.
     */
    virtual void receiveTimer() = 0;

    /**
     * "Sets mask for MDI/O Access Complete Interrupt.",
     * [quote Table 13-65. IMS Register Bit Description]
     * Bit 9 in register.
     */
    virtual void mdioAccessComplete() = 0;

    /**
     * "Sets mask for General Purpose Interrupts.",
     * [quote Table 13-65. IMS Register Bit Description]
     * Bits 14:13 in register.
     * @param value The lowest 2 bits specify the value.
     */
    virtual void generalPurposeInterrupts1(uint8_t value) = 0;

    /**
     * "Sets the mask for Transmit Descriptor Low Threshold hit.",
     * [quote Table 13-65. IMS Register Bit Description]
     * Bit 15 in register.
     */
    virtual void transmitDescriptorLowThresholdHit() = 0;

    /**
     * "Sets mask for Small Receive Packet Detection.",
     * [quote Table 13-65. IMS Register Bit Description]
     * Bit 16 in register.
     */
    virtual void smallReceivePacketDetection() = 0;

    /**
     * Writes selected bits to a given register.
     */
    virtual void manage() = 0;

    /**
     * This methods are not used for Intel 82541IP. For usage
     * please refer to the manual.
     */

    virtual void receiveSequenceError() = 0;
    virtual void transmitDescriptorWrittenBack() = 0;
    virtual void transmitQueueEmpty() = 0;
    virtual void receivingCOrderedSets() = 0;
    virtual void phyInterrupt() = 0;
    virtual void generalPurposeInterrupts2(uint8_t value) = 0;


protected:
    InterruptEnable() = default;

};


#endif //HHUOS_INTERRUPTENABLE_H
