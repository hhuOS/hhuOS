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

#ifndef HHUOS_ERDEFAULT_H
#define HHUOS_ERDEFAULT_H

#include <cstdint>
#include "device/pci/Pci.h"
#include "device/network/e1000/general/Register.h"

#include "EepromRead.h"

/**
 * This abstract class extends the EepromRead
 * interface.
 *
 * All read independent methods are
 * implemented here. Implementing classes have to
 * operate on the varying bit positions.
 */
class ErDefault : public EepromRead {
public:
    ~ErDefault() override = default;

    ErDefault(ErDefault const &) = delete;
    ErDefault &operator=(ErDefault const &) = delete;

protected:
    /**
     * Constructor.
     * @param request The corresponding register to operate on.
     */
    explicit ErDefault(Register *request);

    /**
     * Encapsulates register operations
     * to the Eeprom Read (EERD) register.
     */
    Register *request;

    /**
     * Inherited methods from EepromRead.
     * This methods are meant to be overridden and
     * implemented by extending this class.
     */

    void setAddress(uint8_t address) override = 0;
    void pollDataTransferred() override = 0;

    /**
     * Inherited methods from EepromRead.
     * This methods are meant to be overridden and
     * implemented by this class.
     */

    void clearData() final;
    void setStart() final;
    uint16_t takeData() final;
    void manage() final;
    uint16_t readData(uint8_t address) final;
};


#endif //HHUOS_ERDEFAULT_H
