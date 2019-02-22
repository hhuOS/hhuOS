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
 * 317453006EN.PDF Revision 4.0. 2009
 */

#ifndef HHUOS_REBUFFEREDDEFAULT_H
#define HHUOS_REBUFFEREDDEFAULT_H

#include "ReceiveErrors.h"

class ReBufferedDefault : public ReceiveErrors {
public:
    ~ReBufferedDefault() override = default;

    ReBufferedDefault(ReBufferedDefault const &) = delete;
    ReBufferedDefault &operator=(ReBufferedDefault const &) = delete;

protected:
    /**
     * Constructor.
     * @param address Virtual address of the errors field.
     */
    explicit ReBufferedDefault(uint8_t *address);

    /**
     * Address of the errors field in an descriptor buffer.
     * (address = Buffer-address + position in Buffer).
     */
    volatile uint8_t *address;

    /**
     * The current errors read from the buffer.
     */
    uint8_t errors;

    /**
     * Inherited methods from ReceiveErrors.
     * This methods are meant to be overridden and
     * implemented by extending this class.
     */

    bool symbolError() override = 0;
    bool sequenceError() override = 0;
    bool carrierExtensionError() override = 0;
    bool rxDataError() override = 0;
    bool hasErrors() override = 0;

    /**
     * Inherited methods from ReceiveErrors.
     * This methods are meant to be overridden and
     * implemented by this class.
     */

    bool crcOrAlignmentError() final;
    bool tcpUdpChecksumError() final;
    bool ipChecksumError() final;
    uint8_t loadFromMemory() final;
};


#endif //HHUOS_REBUFFEREDDEFAULT_H
