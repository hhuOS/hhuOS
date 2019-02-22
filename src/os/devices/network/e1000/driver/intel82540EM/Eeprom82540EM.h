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
 * 317453006EN.PDF Revision 4.0. 2009.
 */

#ifndef HHUOS_EEPROM82540EM_H
#define HHUOS_EEPROM82540EM_H


#include "devices/network/e1000/eeprom/ErDefault.h"

/**
 * This class implements the abstract class ErDefault.
 *
 * Register layout:
 *
 *  31  16 15      2      1       0
 * *------*---------*------*-------*
 * | Data | Address | DONE | START |
 * *------*---------*------*-------*
 *
 * [see Table 13-8. EEPROM Read Register Bit Description (82541xx and 82547GI/EI)]
 */
class Eeprom82540EM final : public ErDefault {
public:
    /**
     * Constructor. Same as in extended class.
     */
    explicit Eeprom82540EM(Register *request);
    ~Eeprom82540EM() override = default;

private:
    /**
     * Inherited methods from ErDefault.
     * This methods are meant to be overridden and
     * implemented by this class.
     */

    void pollDataTransferred() final;
    void setAddress(uint8_t address) final;
};


#endif //HHUOS_EEPROM82541IP_H
