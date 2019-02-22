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

#ifndef HHUOS_TSDEFAULT_H
#define HHUOS_TSDEFAULT_H

#include "TransmitStatus.h"

/**
 * This abstract class extends the TransmitStatus
 * interface.
 *
 * For each status bit there is a method, which is used
 * to obtain the status from the descriptor field.
 */
class TsDefault : public TransmitStatus {
public:
    ~TsDefault() override = default;

    TsDefault(TsDefault const &) = delete;
    TsDefault &operator=(TsDefault const &) = delete;

protected:
    /**
     * Initializes the attribute.
     * @param address Virtual address of this field, which points
     * inside the corresponding receive descriptor block.
     */
    explicit TsDefault(uint8_t *address);

    /**
     * Address of the status field in a descriptor buffer.
     * (address = Buffer-address + position in Buffer).
     */
    volatile uint8_t *address;

    /**
     * Inherited method from TransmitStatus.
     * This method is meant to be overridden and
     * implemented by extending this class.
     */

    bool transmitUnderrun() override = 0;

    /**
     * Inherited method from TransmitStatus.
     * This methods are meant to be overridden and
     * implemented by this class.
     */

    bool descriptorDone() final;
    bool excessCollisions() final;
    bool lateCollision() final;
};


#endif //HHUOS_TSDEFAULT_H
