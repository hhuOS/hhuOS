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

#ifndef HHUOS_TRANSMITSTATUS_H
#define HHUOS_TRANSMITSTATUS_H

#include <cstdint>

/**
 * This is an interface for accessing the status field of a transmit descriptor.
 *
 * "The STATUS field stores the applicable transmit descriptor status [...]. The
 *  transmit descriptor status field is only present in cases where RS (or RPS for the 82544GC/EI
 *  only) is set in the command field.",
 *  [quote 3.3.3.2 Transmit Descriptor Status Field Format]
 *
 * Descriptions of the bits are taken from the manual.
 * [see Table 3-11. Transmit Status Layout]
 */
class TransmitStatus {
public:
    virtual ~TransmitStatus() = default;

    TransmitStatus(TransmitStatus const &) = delete;
    TransmitStatus &operator=(TransmitStatus const &) = delete;

    /**
     * "Indicates that the descriptor is finished and is written back either after the descriptor
     *  has been processed (with RS set) or for the 82544GC/EI, after the packet has been
     *  transmitted on the wire (with RPS set).",
     *  [quote Table 3-11. Transmit Status Layout]
     * Bit 0 in field.
     * @return True, if the descriptor is done. Otherwise false.
     */
    virtual bool descriptorDone() = 0;

    /**
     * "Indicates that the packet has experienced more than the maximum excessive
     *  collisions as defined by TCTL.CT control field and was not transmitted. It has no
     *  meaning while working in full-duplex mode.",
     *  [quote Table 3-11. Transmit Status Layout]
     *
     * Bit 1 in field.
     * @return True, if there were excessive collisions. Otherwise false.
     */
    virtual bool excessCollisions() = 0;


    /**
     * "Indicates that late collision occurred while working in half-duplex mode. It has no
     *  meaning while working in full-duplex mode. Note that the collision window is speed
     *  dependent: 64 bytes for 10/100 Mb/s and 512 bytes for 1000 Mb/s operation.",
     *  [quote Table 3-11. Transmit Status Layout]
     *
     * Bit 2 in field.
     * @return True, if it occurred a late collision. Otherwise false.
     */
    virtual bool lateCollision() = 0;

    /**
     * This method is not used for Intel 82541IP. For usage
     * please refer to the manual.
     */

    virtual bool transmitUnderrun() = 0;

protected:
    TransmitStatus() = default;
};


#endif //HHUOS_TRANSMITSTATUS_H
