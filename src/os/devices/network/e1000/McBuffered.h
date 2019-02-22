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

#ifndef HHUOS_MCBUFFERED_H
#define HHUOS_MCBUFFERED_H

#include "Mac.h"
/**
 * This class implementis the Mac interface.
 *
 * MAC-addresses can be loaded from EEPROM or MMIO-space as well
 * as be written to a given buffer. Classes who want to send a
 * packet and build an ethernet frame should call this class and
 * into the Mac-interface.
 */
class McBuffered final : public Mac {
public:
    explicit McBuffered() = default;
    ~McBuffered() override = default;

private:
    /**
     * The MAC-address, where the bytes are sorted
     * in ascending order according to the indexes.
     * Index              5   4  3  2  1       0
     * MAC-address: (MSB 05) 04 03 02 01 (LSB 00)
     */
    uint8_t mac[6]{0};

    /**
      * Inherited methods from Mac.
      * This methods are meant to be overridden and
      * implemented by this class.
      */
    uint8_t *read(EepromRead *eepromRead) final;
    uint8_t *read(uint8_t *mioBase) final;
    void writeTo(uint8_t *buffer, uint32_t position) final;
    void getByteRepresentation(uint8_t *buf) final;
    String getStringRepresentation() final;
};


#endif //HHUOS_MCBUFFERED_H
