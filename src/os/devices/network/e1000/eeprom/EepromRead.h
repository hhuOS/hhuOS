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

#ifndef HHUOS_EEPROMREAD_H
#define HHUOS_EEPROMREAD_H


#include <cstdint>

/**
 * This is an interface for handling reads from the EEPROM.
 * To control the EEPROM the register EERD mapped to MMIO-space is used.
 *
 * "EERD (00014h; RW)
 *
 *  This register is used by software to cause the Ethernet controller to read individual words in the
 *  EEPROM. To read a word, software writes the address to the Read Address field and simultaneously
 *  writes a 1b to the Start Read field. The Ethernet controller reads the word from the EEPROM
 *  and places it in the Read Data field, setting the Read Done filed to 1b. Software can poll this
 *  register, looking for a 1b in the Read Done filed, and then using the value in the Read Data field.
 *
 *  When this register is used to read a word from the EEPROM, that word is not written to any of
 *  Ethernet controller’s internal registers even if it is normally a hardware accessed word.",
 *  [quote 13.4.4 EEPROM Read Register]
 *
 * General layout:
 *
 *  31  16 15           1       0
 * *------*--------------*-------*
 * | Data | depend on EC | START |
 * *------*--------------*-------*
 *
 * Descriptions of the bits are taken from the manual.
 * [see Table 13-7. EEPROM Read Register Bit Description]
 * [see Table 13-8. EEPROM Read Register Bit Description (82541xx and 82547GI/EI)]
 *
 * [see Table 5-2. Ethernet Controller Address Map]
 */
class EepromRead {
public:
    virtual ~EepromRead() = default;

    EepromRead(EepromRead const &) = delete;
    EepromRead &operator=(EepromRead const &) = delete;

    /**
     * Fetches data from the EEPROM.
     * @param address Offset into the corresponding address map.
     * @return Word from the EEPROM.
     */
    virtual uint16_t readData(uint8_t address) = 0;

protected:
    EepromRead() = default;

private:
    /**
     * Sets the Data-bits to 0. This is recommended before each read, to ensure
     * functionality.
     */
    virtual void clearData() = 0;

    /**
     * Sets the start bit to trigger read from EEPROM.
     */
    virtual void setStart() = 0;

    /**
     * Reads the Word from the EERD register which was fetched by
     * the EEPROM read.
     * @return The Word stored in Data region.
     */
    virtual uint16_t takeData() = 0;

    /**
     * Applies all set bits at once to the corresponding register.
     */
    virtual void manage() = 0;

    /**
     * Stores a given address to the EERD register, so that the EEPROM
     * fetch algorithm knows what to fetch.
     * @param address The offset into the corresponding address map.
     */
    virtual void setAddress(uint8_t address) = 0;

    /**
     * Reads the DONE bit to verify if the hardware has fetched
     * the data and is done. The driver stalls until then.
     */
    virtual void pollDataTransferred() = 0;
};


#endif //HHUOS_EEPROMREAD_H
