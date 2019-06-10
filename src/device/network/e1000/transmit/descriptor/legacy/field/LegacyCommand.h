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

#ifndef HHUOS_LEGACYCOMMAND_H
#define HHUOS_LEGACYCOMMAND_H


#include <cstdint>
/**
 * This is an interface for accessing and setting the command field of a transmit descriptor.
 *
 *      7     6      5          4    3    2      1     0
 * *-----*-----*------*----------*----*----*------*-----*
 * | IDE | VLE | DEXT | RSV./RPS | RS | IC | IFCS | EOP |
 * *-----*-----*------*----------*----*----*------*-----*
 *
 * "Note:
 *   VLE, IFCS, and IC are qualified by EOP. That is, hardware interprets these bits ONLY when
 *   EOP is set.
 *
 *   Hardware only sets the DD bit for descriptors with RS set.
 *
 *   Descriptors with the null address (0b) or zero length transfer no data. If they have the RS bit
 *   set then the DD field in the status word is written when hardware processes them.
 *
 *   Although the transmit interrupt may be delayed, the descriptor write-back requested by setting
 *   the RS bit is performed without delay unless descriptor write-back bursting is enabled.",
 *   [quote 3.3.3.1 Transmit Descriptor Command Field Format]
 *
 * Descriptions of the bits are taken from the manual.
 * [see Table 3-10. Transmit Command (TDESC.CMD) Layout]
 */
class LegacyCommand {
public:
    virtual ~LegacyCommand() = default;

    LegacyCommand(LegacyCommand const &) = delete;
    LegacyCommand &operator=(LegacyCommand const &) = delete;

    /**
     * "When set, indicates the last descriptor making up the packet. One or many
     *  descriptors can be used to form a packet."
     *  [quote Table 3-10. Transmit Command (TDESC.CMD) Layout]
     *
     * Bit 0 in register.
     * @param enable If true, the named functionality will be activated.
     */
    virtual void isEndOfPacket(bool enable) = 0;

    /**
     * "Controls the insertion of the FCS/CRC field in normal Ethernet packets. [...] IFCS is
     *  valid only when EOP is set.",
     *  [quote Table 3-10. Transmit Command (TDESC.CMD) Layout]
     *
     * Bit 1 in register.
     * @param enable If true, the named functionality will be activated.
     */
    virtual void insertFrameCheckSequence(bool enable) = 0;

    /**
     * "When set, the Ethernet controller needs to report the status information. This ability
     *  may be used by software that does in-memory checks of the transmit descriptors to
     *  determine which ones are done and packets have been buffered in the transmit
     *  FIFO. Software does it by looking at the descriptor status byte and checking the
     *  Descriptor Done (DD) bit.",
     *  [quote Table 3-10. Transmit Command (TDESC.CMD) Layout]
     *
     * Bit 3 in register.
     * @param enable If true, the named functionality will be activated.
     */
    virtual void reportStatus(bool enable) = 0;

    /**
     * "Extension (0b for legacy mode).
     *  Should be written with 0b for future compatibility.",
     *  [quote Table 3-10. Transmit Command (TDESC.CMD) Layout]
     *
     * Bit 5 in register.
     * @param enable If false, enables legacy mode.
     */
    virtual void legacyMode(bool enable) = 0;

    /**
     * Writes selected bits to a given register.
     */
    virtual void manage() = 0;

    /**
     * This methods are not used for Intel 82541IP. For usage
     * please refer to the manual.
     */

    virtual void insertChecksum(bool enable) = 0;
    virtual void reportPacketSent(bool enable) = 0;
    virtual void enableVlanPacket(bool enable) = 0;
    virtual void enableInterruptDelay(bool enable) = 0;

protected:
    LegacyCommand() = default;
};


#endif //HHUOS_LEGACYCOMMAND_H
