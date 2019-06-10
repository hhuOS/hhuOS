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

#ifndef HHUOS_LCDEFAULT_H
#define HHUOS_LCDEFAULT_H

#include "device/cpu/Cpu.h"
#include "device/network/e1000/general/BitManipulation.h"

#include "LegacyCommand.h"

/**
 * This abstract class extends the LegacyCommand
 * interface.
 *
 * Collect options due using this methods and apply them
 * with the manage method to the register.
 */
class LcDefault : public LegacyCommand {
public:
    ~LcDefault() override = default;

    LcDefault(LcDefault const &) = delete;
    LcDefault &operator=(LcDefault const &) = delete;

protected:
    /**
     * Constructor.
     * @param address Virtual address of the field according to MMIO-space.
     * @param manipulation Manipulator for the command sequence.
     */
    explicit LcDefault(uint8_t *address, BitManipulation<uint8_t> *manipulation);

    /**
     * The address of the command field in the descriptor buffer.
     * (address = Buffer-address + position in Buffer).
     */
    volatile uint8_t *address;

    /**
     * Encapsulates access to the command field.
     */
    BitManipulation<uint8_t> *manipulation;

    /**
     * Inherited methods from LegacyCommand.
     * This methods are meant to be overridden and
     * implemented by extending this class.
     */

    void reportPacketSent(bool enable) override = 0;

    /**
     * Inherited methods from LegacyCommand.
     * This methods are meant to be overridden and
     * implemented by this class.
     */

    void isEndOfPacket(bool enable) final;
    void insertFrameCheckSequence(bool enable) final;
    void insertChecksum(bool enable) final;
    void reportStatus(bool enable) final;
    void legacyMode(bool enable) final;
    void enableVlanPacket(bool enable) final;
    void enableInterruptDelay(bool enable) final;
    void manage() final;
};


#endif //HHUOS_LCDEFAULT_H
