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

#ifndef HHUOS_INTERRUPTDISABLE_H
#define HHUOS_INTERRUPTDISABLE_H

/**
 * This interface provides methods for disabling individual interrupts.
 * To disable interrupts the register Interrupt Mask Clear (IMC), which is mapped to MMIO-space, is used.
 *
 * "IMC (000D8h; W)
 *
 *  Software uses this register to disable an interrupt. Interrupts are presented to the bus interface only
 *  when the mask bit is set to 1b and the cause bit set to 1b. The status of the mask bit is reflected in
 *  the Interrupt Mask Set/Read Register (see Section 13.4.20), and the status of the cause bit is
 *  reflected in the Interrupt Cause Read Register (see Section 13.4.17).
 *
 *  Software blocks interrupts by clearing the corresponding mask bit. This is accomplished by writing
 *  a 1b to the corresponding bit in this register. Bits written with 0b are unchanged (their mask status
 *  does not change).
 *
 *  Software Developer’s ManualRegister Descriptions
 *  Software should write a 1b to the reserved bits to ensure future compatibility. Since this register
 *  masks interrupts when 1b is written to the corresponding (defined) bits, then writing 1b to the
 *  reserved bits ensures that the software is never called to handle an interrupt that the software is not
 *  aware exists.",
 *  [quote 13.4.21 Interrupt Mask Clear Register]
 *
 *  31      17 16       0
 * *----------*----------*
 * | Reserved | IMC Bits |
 * *----------*----------*
 *
 * Descriptions of the bits are taken from the manual.
 * [see Table 13-66. IMC Register Bit Description]
 *
 * Note:
 * There is a special situation for the 82547GI/EI, described by the manual.
 */
class InterruptDisable {
public:
    virtual ~InterruptDisable() = default;

    InterruptDisable(InterruptDisable const &) = delete;
    InterruptDisable &operator=(InterruptDisable const &) = delete;

    /**
     * Writes 1b to bits 31:17,11,8,5 for future compatibility.
     */
    virtual void clearReserved() = 0;

    /**
     * Writes selected bits to a given register.
     */
    virtual void manage() = 0;

    /**
     * This methods are not used for Intel 82541IP. For usage
     * please refer to the manual.
     */

    virtual void transmitDescriptorWrittenBack() = 0;
    virtual void transmitQueueEmpty() = 0;
    virtual void linkStatusChange() = 0;
    virtual void receiveSequenceError() = 0;
    virtual void receiveDescriptorMinimumThresholdHit() = 0;
    virtual void receiverFifoOverrun() = 0;
    virtual void receiveTimer() = 0;
    virtual void mdioAccessComplete() = 0;
    virtual void receivingCOrderedSets() = 0;
    virtual void phyInterrupt() = 0;
    virtual void generalPurposeInterrupts2(uint8_t value) = 0;
    virtual void generalPurposeInterrupts1(uint8_t value) = 0;
    virtual void transmitDescriptorLowThresholdHit() = 0;
    virtual void smallReceivePacketDetection() = 0;

protected:
    InterruptDisable() = default;
};


#endif //HHUOS_INTERRUPTDISABLE_H
