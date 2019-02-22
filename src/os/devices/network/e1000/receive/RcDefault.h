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
 * All references marked with [...] refer to  following developers manual.
 * Intel Corporation. PCI/PCI-X Family of Gigabit Ethernet Controllers Software Developer’s Manual.
 * 317453006EN.PDF Revision 4.0. 2009
 */

#ifndef HHUOS_RCDEFAULT_H
#define HHUOS_RCDEFAULT_H

#include <devices/cpu/Cpu.h>
#include "devices/network/e1000/general/Register.h"

#include "ReceiveControl.h"


/**
 * This abstract class extends the ReceiveControl
 * interface.
 *
 * Collect options due using this methods and apply them
 * with the manage method to the register.
 */
class RcDefault : public ReceiveControl {
public:
    ~RcDefault() override = default;

    RcDefault(RcDefault const &) = delete;
    RcDefault &operator=(RcDefault const &) = delete;

protected:
    /**
     * Initializes the attribute.
     * @param request Contains the register address and
     * encapsulates manipulations to it.
     */
    explicit RcDefault(Register *request);

    /**
     * Encapsulates register operations
     * to the receive control register.
     */
    Register *request;

    /**
     * Inherited methods from ReceiveControl.
     * This methods are meant to be overridden and
     * implemented by extending this class.
     */

    void loopbackMode(uint8_t value) override = 0;
    void vlanFilter(bool enable) override = 0;

    /**
     * Inherited methods from ReceiveControl.
     * This methods are meant to be overridden and
     * implemented by this class.
     */

    void receiver(bool enable) final;
    void storeBadPackets(bool enable) final;
    void unicastPromiscuous(bool enable) final;
    void multicastPromiscuous(bool enable) final;
    void longPacketReception(bool enable) final;
    void descriptorMinimumThresholdSize(uint8_t value) final;
    void multicastOffset(uint8_t value) final;
    void broadCastAcceptMode(bool enable) final;
    void bufferSize(uint16_t value) final;
    void canonicalFormIndicator(bool enable) final;
    void canonicalFormIndicatorBitValue(bool enable) final;
    void discardPauseFrames(bool enable) final;
    void passMacControlFrames(bool enable) final;
    void bufferSizeExtension(bool enable) final;
    void stripEthernetCrc(bool enable) final;
    void manage() final;
};


#endif //HHUOS_RCDEFAULT_H
