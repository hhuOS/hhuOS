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
 */

#ifndef HHUOS_RECEIVEDESCRIPTORSIMPLE_H
#define HHUOS_RECEIVEDESCRIPTORSIMPLE_H

#include "RdDefault.h"

class ReceiveDescriptorSimple final : public RdDefault {
public:
    /**
      * Constructor. Same as in extended class.
      */
    explicit ReceiveDescriptorSimple(uint8_t *address, uint8_t *packetBufferAddress,
                                         ReceiveErrors *errors, ReceiveStatus *status);
    ~ReceiveDescriptorSimple() override = default;

private:
    /**
     * Inherited methods from RdDefault.
     * This methods are meant to be overridden and
     * implemented by extending this class.
     */
    uint16_t readPacketChecksum() final;
    uint16_t readSpecial() final;
};


#endif //HHUOS_RECEIVEDESCRIPTORSIMPLE_H
