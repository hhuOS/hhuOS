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

#ifndef HHUOS_DCDEFAULT_H
#define HHUOS_DCDEFAULT_H

#include "Register.h"
#include "DeviceControl.h"

/**
 * This abstract class extends the DeviceControl
 * interface.
 *
 * Collect options due using this methods and apply them
 * with the manage method to the register.
 */
class DcDefault : public DeviceControl {
public:
    ~DcDefault() override = default;

    DcDefault(DcDefault const &) = delete;
    DcDefault &operator=(DcDefault const &) = delete;

protected:
    /**
     * Constructor.
     * @param request Contains the register address and
     * encapsulates manipulations to it.
     */
    explicit DcDefault(Register *request);

    /**
     * Encapsulates register operations
     * to the device control register.
     */
    Register *request;

    /**
     * Inherited methods from DeviceControl.
     * This methods are meant to be overridden and
     * implemented by extending this class.
     */

    void linkReset(bool enable) override = 0;
    void invertLossOfSignal(bool enable) override = 0;
    void wakeupAdvertisement(bool enable) override = 0;
    void vlanMode(bool enable) override = 0;
    void resetInternalPhy(bool enable) override = 0;

    /**
     * Inherited methods from DeviceControl.
     * This methods are meant to be overridden and
     * implemented by this class.
     */

    void fullDuplex(bool enable) final;
    void autoSpeedDetection(bool enable) final;
    void setLinkUp(bool enable) final;
    void speedSelection(uint8_t value) final;
    void forceSpeed(bool enable) final;
    void forceDuplex(bool enable) final;
    void sdp0Data(bool mode) final;
    void sdp1Data(bool mode) final;
    void phyPowerManagement(bool enable) final;
    void sdp0IoDirection(bool direction) final;
    void sdp1IoDirection(bool direction) final;
    void deviceReset(bool enable) final;
    void flowControlReceive(bool enable) final;
    void flowControlTransmit(bool enable) final;
    void manage() final;
};


#endif //HHUOS_DCDEFAULT_H
