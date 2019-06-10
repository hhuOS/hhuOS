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

#ifndef HHUOS_INTERRUPTCAUSE_H
#define HHUOS_INTERRUPTCAUSE_H

#include <cstdint>

/**
 * This is an interface determining the kinds of interrupts thrown.
 * To accomplish this, the driver should fetch the interrupt-vector once,
 * and operate on it, rather than reading the register for each single interrupt.
 * The interrupt-vector will be read from the ICR register, which is mapped to the MMIO-space.
 *
 * "ICR (000C0H; R)
 *
 *  This register contains all interrupt conditions for the Ethernet controller. Each time an interrupt
 *  causing event occurs, the corresponding interrupt bit is set in this register. A PCI interrupt is
 *  generated each time one of the bits in this register is set, and the corresponding interrupt is enabled
 *  through the Interrupt Mask Set/Read IMS Register (see Section 13.4.20).
 *
 *  All register bits are cleared upon read. As a result, reading this register implicitly acknowledges
 *  any pending interrupt events. Writing a 1b to any bit in the register also clears that bit. Writing a 0b
 *  to any bit has no effect on that bit.",
 *  [quote 13.4.17 Interrupt Cause Read Register]
 *
 *  31      17 14       0
 * *----------*----------*
 * | Reserved | ICR Bits |
 * *----------*----------*
 *
 * Descriptions of the bits are taken from the manual.
 * [see Table 13-63. ICR Register Bit Description]
 *
 * Note:
 * There is a special situation for the 82547GI/EI, described by the manual.
 */
class InterruptCause {
public:
    virtual ~InterruptCause() = default;

    InterruptCause(InterruptCause const &) = delete;
    InterruptCause &operator=(InterruptCause const &) = delete;

    /**
     * Fetches the current interrupt-values and clears it.
     * @return Content of the corresponding register.
     */
    virtual uint32_t readAndClear() = 0;

    /**
     * Each time the link status changes, this bit is set.
     * Bit 2 in register.
     * @return True if the link status has changed, false otherwise.
     */
    virtual bool hasLinkStatusChanged() = 0;

    /**
     * The minimum number of receive descriptors are
     * available. If set, The driver should load more receive descriptors.
     * Bit 4 in register.
     * @return True if the receive descriptor minimum threshold is reached.
     */
    virtual bool isReceiveDescriptorMinimumThresholdReached() = 0;

    /**
     * Set when there is a receive data FIFO overrun.
     * There are two different reasons, that could have happen.
     * Either there are no available receive buffers or the PCI
     * receive bandwidth is inadequate.
     * Bit 6 in register.
     * @return True if there is an overrun of the receiver, false otherwise.
     */
    virtual bool isReceiverOverrun() = 0;

    /**
     * "Set when the receiver timer expires.
     *  The receiver timer is used for receiver descriptor packing. Timer
     *  expiration flushes any accumulated descriptors and sets an
     *  interrupt event when enabled.",
     * [quote Table 13-63. ICR Register Bit Description]
     *
     * Bit 7 in register.
     * @return True if the receiver timer expired, false otherwise.
     */
    virtual bool hasReceiveTimerInterrupt() = 0;

    /**
     * Checks, whether all asserted interrupts were handled or not.
     * @return True, if there is at least one unhandled interrupt.
     */
    virtual bool hasUnhandledInterrupts() = 0;

    /**
     * This method should used for diagnostic purposes only.
     * @return Current state of buffered interrupts.
     * Some may be handled already.
     */
    virtual uint32_t getInterrupts() = 0;

    /**
     * This methods are not used for Intel 82541IP. For usage
     * please refer to the manual.
     */

    virtual bool hasReceiveSequenceError() = 0;
    virtual bool isTransmitDescriptorWrittenBack() = 0;
    virtual bool isTransmitQueueEmpty() = 0;
    virtual bool isMdioAccessCompleted() = 0;
    virtual bool hasReceivingCOrderedSets() = 0;
    virtual bool isPhyInterrupt() = 0;
    virtual uint8_t generalPurposeInterrupts_1() = 0;
    virtual bool isGeneralPurposeInterruptOnSDP6_2() = 0;
    virtual bool isGeneralPurposeInterruptOnSDP7_3() = 0;
    virtual bool isTransmitDescriptorLowThresholdHit() = 0;
    virtual bool hasSmallReceivePacketDetected() = 0;

protected:
    InterruptCause() = default;
};


#endif //HHUOS_INTERRUPTCAUSE_H
