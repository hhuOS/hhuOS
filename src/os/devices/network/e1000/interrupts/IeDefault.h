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

#ifndef HHUOS_IEDEFAULT_H
#define HHUOS_IEDEFAULT_H

#include "devices/network/e1000/general/Register.h"
#include "InterruptEnable.h"

/**
 * This abstract class extends the InterruptEnable
 * interface.
 *
 * Collect options due using this methods and apply them
 * with the manage method to the register.
 */
class IeDefault : public InterruptEnable {
public:
    ~IeDefault() override = default;

    IeDefault(IeDefault const &) = delete;
    IeDefault &operator=(IeDefault const &) = delete;

protected:
    /**
     * Constructor.
     * @param request  Contains the register address and
     * encapsulates manipulations to it.
     */
    explicit IeDefault(Register *request);

    /**
     * Encapsulates register operations to the
     * interrupt mask set register.
     */
    Register *request;

    /**
     * Inherited method from InterruptEnable.
     * This methods are meant to be overridden and
     * implemented by extending this class.
     */

    void receiveSequenceError() override = 0;
    void receivingCOrderedSets() override = 0;
    void phyInterrupt() override = 0;
    void generalPurposeInterrupts2(uint8_t value) override = 0;
    void transmitDescriptorLowThresholdHit() override = 0;
    void smallReceivePacketDetection() override = 0;

    /**
     * Inherited methods from InterruptEnable.
     * This methods are meant to be overridden and
     * implemented by this class.
     */

    void transmitDescriptorWrittenBack() final;
    void transmitQueueEmpty() final;
    void linkStatusChange() final;
    void receiveDescriptorMinimumThresholdHit() final;
    void receiverFifoOverrun() final;
    void receiveTimer() final;
    void mdioAccessComplete() final;
    void generalPurposeInterrupts1(uint8_t value) final;
    void manage() final;
};


#endif //HHUOS_IEDEFAULT_H
