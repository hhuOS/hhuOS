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

#ifndef HHUOS_DEVICECONTROL_H
#define HHUOS_DEVICECONTROL_H

#include <cstdint>

/**
 * This is an interface for handling basic configurations of the device.
 * To control the device the register device control (CTRL), mapped to MMIO-space, is used.
 *
 * "CTRL (00000h; R/W)
 *
 *  This register and the Extended Device Control register (CTRL_EXT) control the major operational
 *  modes for the Ethernet controller.
 *  While software writes to this register to control device settings, several bits (such as FD and
 *  SPEED) can be overridden depending on other bit settings and the resultant link configuration
 *  determined by the PHY’s Auto-Negotiation resolution.
 *
 *  The ADVD3WUC bit (Advertise D3Cold Wakeup Capability Enable control) allows the
 *  AUX_PWR pin to determine whether D3Cold support is advertised. If full 1 Gb/s operation in D3
 *  state is desired but the system’s power requirements in this mode would exceed the D3Cold
 *  Wakeup-Enabled specification limit (375 mA at 3.3 V dc), this bit can be used to prevent the
 *  capability from being advertised to the system.
 *
 *  EEPROM settings allow the default PHY behavior to re-negotiate a lower functional link speed in
 *  D3 and D0u states, when PHY operation is still needed for manageability or wakeup capability.
 *  The EN_PHY_PWR_MGMT bit allows this capability to be disabled, in case full 1Gb/s speed is
 *  desired in these states. The PHY is always powered-down in D3 states when unneeded for either
 *  manageability or wakeup support.",
 *  [quote 13.4.1 Device Control Register]
 *
 * Descriptions of the bits are taken from the manual.
 * [see Table 13-3. CTRL Register Bit Description]
 */
class DeviceControl {
public:
    virtual ~DeviceControl() = default;

    DeviceControl(DeviceControl const &) = delete;
    DeviceControl &operator=(DeviceControl const &) = delete;

    /**
     * Enables auto-speed detection.
     * Sets Bit 5 in register.
     * @param enable If true enables functionality given by name, when
     * the setLinkUp method has set its bit.
     */
    virtual void autoSpeedDetection(bool enable) = 0;

    /**
     * Sets the link up.
     * Sets Bit 6 in register. Has to be set for enabling auto-speed detection.
     * @param enable If true enables functionality given by name.
     */
    virtual void setLinkUp(bool enable) = 0;

    /**
     * Applies all set bits at once to the corresponding register.
     */
    virtual void manage() = 0;

    /**
     * This Methods are not used for Intel 82541IP. For usage
     * please refer to the manual.
     */

    virtual void fullDuplex(bool enable) = 0;
    virtual void linkReset(bool enable) = 0;
    virtual void invertLossOfSignal(bool enable) = 0;
    virtual void speedSelection(uint8_t value) = 0;
    virtual void forceSpeed(bool enable) = 0;
    virtual void forceDuplex(bool enable) = 0;
    virtual void sdp0Data(bool mode) = 0;
    virtual void sdp1Data(bool mode) = 0;
    virtual void wakeupAdvertisement(bool enable) = 0;
    virtual void phyPowerManagement(bool enable) = 0;
    virtual void sdp0IoDirection(bool direction) = 0;
    virtual void sdp1IoDirection(bool direction) = 0;
    virtual void deviceReset(bool enable) = 0;
    virtual void flowControlReceive(bool enable) = 0;
    virtual void flowControlTransmit(bool enable) = 0;

    /**
     * This Methods are only used for Intel 82541IP,
     * to ensure, that the corresponding bits are set off.
     * For other usage please refer to the manual.
     */

    virtual void vlanMode(bool enable) = 0;
    virtual void resetInternalPhy(bool enable) = 0;

protected:
    DeviceControl() = default;
};


#endif //HHUOS_DEVICECONTROL_H
