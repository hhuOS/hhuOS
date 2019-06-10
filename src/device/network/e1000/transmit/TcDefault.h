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

#ifndef HHUOS_TCDEFAULT_H
#define HHUOS_TCDEFAULT_H

#include "device/network/e1000/general/Register.h"
#include "TransmitControl.h"

/**
 * This abstract class extends the TransmitControl
 * interface.
 *
 * Collect options due using this methods and apply them
 * with the manage method to the register.
 */
class TcDefault : public TransmitControl {
public:
    ~TcDefault() override = default;

    TcDefault(TcDefault const &) = delete;
    TcDefault &operator=(TcDefault const &) = delete;

protected:
    /**
     * Constructor.
     * @param request Contains the register address and
     * encapsulates manipulations to it.
     */
    explicit TcDefault(Register *request);

    /**
     * Encapsulates register operations
     * to the transmit control register.
     */
    Register *request;

    /**
     * Inherited methods from TransmitControl.
     * This methods are meant to be overridden and
     * implemented by extending this class.
     */

    void noRetransmitOnUnderrun(bool enable) override = 0;

    /**
     * Inherited methods from TransmitControl.
     * This methods are meant to be overridden and
     * implemented by this class.
     */

    void enableTransmit(bool enable) final;
    void padShortPackets(bool enable) final;
    void collisionThreshold(uint8_t value) final;
    void collisionDistance(uint16_t value) final;
    void softwareXOffTransmission(bool enable) final;
    void retransmitOnLateCollision(bool enable) final;
    void manage() final;
};


#endif //HHUOS_TCDEFAULT_H
