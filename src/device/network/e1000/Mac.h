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

#ifndef HHUOS_MAC_H
#define HHUOS_MAC_H

#include "device/pci/Pci.h"
#include <cstdint>

#include "eeprom/EepromRead.h"

/**
 * This is an Interface for handling Mac-Addresses.
 */
class Mac {
public:
    virtual ~Mac() = default;

    Mac(Mac const &) = delete;
    Mac &operator=(Mac const &) = delete;

    /**
     * Read the MAC-address from EEPROM.
     * @param eepromRead The EEPROM interface to communicate with.
     * @return Byte-Array with the MAC-address stored in it.
     */
    virtual uint8_t *read(EepromRead *eepromRead) = 0;

    /**
     * Read the MAC-address from MMIO-Space.
     * @param mioBase The start address of the MAC-address stored in MMIO-Space.
     * @return Byte-Array with the MAC-address stored in it.
     */
    virtual uint8_t *read(uint8_t *mioBase) = 0;

    /**
     * Writes the MAC-Address to the given address.
     * @param buffer The address where to write the MAC-address.
     */
    virtual void writeTo(uint8_t *buffer, uint32_t position) = 0;

    /**
     * Copies the MAC-address into a given byte array.
     * @param mac The byte array to copy the MAC-address to
     */
    virtual void getByteRepresentation(uint8_t *buf) = 0;

    /**
     * Converts the MAC-address to a String.
     * @return String-representation of the MAC-address.
     */
    virtual String getStringRepresentation() = 0;

protected:
    Mac() = default;
};


#endif //HHUOS_MAC_H
