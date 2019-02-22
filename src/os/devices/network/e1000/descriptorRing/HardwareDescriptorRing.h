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

#ifndef HHUOS_HARDWAREDESCRIPTORRING_H
#define HHUOS_HARDWAREDESCRIPTORRING_H


#include <cstdint>
/**
 * This is an interface for transmit/receive-ring on the hardware.
 * It is used to initialize it due writing to specific
 * registers, mapped to the MMIO-space.
 *
 * Ring-register Block Structure:
 *  0x00 Descriptor Base Low (DBAL)
 *  0x04 Descriptor Base High (DBAH)
 *  0x08 Descriptor Base Length (DBLEN)
 *  0x10 Descriptor Head (DH)
 *  0x18 Descriptor Tail (DT)
 * [see Table 13-2. Ethernet Controller Register Summary]
 *
 * The hex-values are offsets to be added to the start of the
 * mapped registers of the rings. The receive-ring starts at address 0x02800 and the
 * transmit-ring at address 0x03800.
 */
class HardwareDescriptorRing {
public:
    virtual ~HardwareDescriptorRing() = default;

    HardwareDescriptorRing(HardwareDescriptorRing const &) = delete;
    HardwareDescriptorRing &operator=(HardwareDescriptorRing const &) = delete;

    /**
     * Increases the tail register by 1 and wraps it back to 0, if
     * the size of the ring (amount of descriptors in the ring) has
     * been reached.
     */
    virtual void updateTail() = 0;

    /**
     * Calls all the initializing methods needed for initializing
     * the whole ring.
     */
    virtual void initialize() = 0;

protected:
    HardwareDescriptorRing() = default;

    /**
     * Initializes descriptor base address registers (DBAL and DBAH).
     *
     * Offset
     *         31   4 3 0
     *        *------*---*
     * 0x00   | DBAL | 0 |
     *        *------*---*
     * 0x04   |   DBAH   |
     *        *----------*
     * [see 13.4.25 Receive Descriptor Base Address Low]
     * [see 13.4.26 Receive Descriptor Base Address High]
     */
    virtual void initBase() = 0;

    /**
     * Initializes the descriptors head register (DH).
     *
     *  31  16 15 0
     * *------*----*
     * | RSV. | DH |
     * *------*----*
     *
     * [see 13.4.28 Receive Descriptor Head]
     */
    virtual void initHead() = 0;

    /**
     * Initializes Descriptor Length register (DLEN).
     *
     *  31  20 19  7 6 0
     * *------*-----*---*
     * | RSV. | LEN | 0 |
     * *------*-----*---*
     *
     * [see 13.4.27 Receive Descriptor Length]
     */
    virtual void initLength() = 0;

    /**
     * Initializes Descriptor Tail register (DT)
     *
     *  31  16 15 0
     * *------*----*
     * | RSV. | DT |
     * *------*----*
     *
     * [see 13.4.29 Receive Descriptor Tail]
     */
    virtual void initTail() = 0;

    /**
     * Calculates the address of a ring-register.
     * Software should use this method to select a certain register
     * from within the init-methods and set them to appropriate values.
     * @param number offset into the transmit/receive register block
     * @return Pointer to a ring-register chosen by number
     */
    virtual uint32_t *chooseRegister(uint8_t number) = 0;
};


#endif //HHUOS_HARDWAREDESCRIPTORRING_H
