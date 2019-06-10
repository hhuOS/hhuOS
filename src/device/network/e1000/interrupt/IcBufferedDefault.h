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

#ifndef HHUOS_ICBUFFEREDDEFAULT_H
#define HHUOS_ICBUFFEREDDEFAULT_H

#include "InterruptCause.h"

/**
 * This abstract class extends the InterruptCause
 * interface.
 *
 * With this class one fetches an interrupt-vector, progresses and
 * finally clears it.
 */
class IcBufferedDefault : public InterruptCause {
public:
    ~IcBufferedDefault() override = default;

    IcBufferedDefault(IcBufferedDefault const &) = delete;
    IcBufferedDefault &operator=(IcBufferedDefault const &) = delete;

protected:
    /**
     * Constructor. The attribute 'interrupts' will be
     * defaulted with 0.
     * @param address Address of the interrupt cause register in MMIO-space.
     */
    explicit IcBufferedDefault(uint32_t *address);

    /**
     * Address of the interrupt cause register in MMIO-space.
     * (address = bar-address + offset in MMIO-space).
     */
    volatile uint32_t *address;

    /**
     * Complete value of the ICR register.
     * Will be updated for each handled interrupt.
     */
    uint32_t interrupts;

    /**
     * Sets bit of handled interrupt to 0b.
     * @param interruptNumber Position of bit according to the handled interrupt.
     */
    void setInterruptHandled(uint8_t interruptNumber);

    /**
     * Checks if an interrupt was asserted. If it was, it will be
     * set to 0b in the buffered interrupts-value.
     * @param interruptNumber Bit position of the interrupt.
     * @return True, if the interrupt was asserted.
     */
    bool processInterrupt(uint8_t interruptNumber);

    /**
     * Inherited method from InterruptCause.
     * This methods are meant to be overridden and
     * implemented by extending this class.
     */

    uint8_t generalPurposeInterrupts_1() override = 0;
    bool hasReceiveSequenceError() override = 0;
    bool hasReceivingCOrderedSets() override = 0;
    bool isPhyInterrupt() override = 0;
    bool isGeneralPurposeInterruptOnSDP6_2() override = 0;
    bool isGeneralPurposeInterruptOnSDP7_3() override = 0;
    bool isTransmitDescriptorLowThresholdHit() override = 0;
    bool hasSmallReceivePacketDetected() override = 0;

private:
    /**
     * Inherited method from InterruptCause.
     * This methods are meant to be overridden and
     * implemented by this class.
     */

    uint32_t readAndClear() final;
    bool isTransmitDescriptorWrittenBack() final;
    bool isTransmitQueueEmpty() final;
    bool hasLinkStatusChanged() final;
    bool isReceiveDescriptorMinimumThresholdReached() final;
    bool isReceiverOverrun() final;
    bool hasReceiveTimerInterrupt() final;
    bool isMdioAccessCompleted() final;
    bool hasUnhandledInterrupts() final;
    uint32_t getInterrupts() final;
};


#endif //HHUOS_ICBUFFEREDDEFAULT_H
