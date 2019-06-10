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

#ifndef HHUOS_RDDEFAULT_H
#define HHUOS_RDDEFAULT_H

#include "field/ReceiveErrors.h"
#include "field/ReceiveStatus.h"

#include "ReceiveDescriptor.h"

/**
 * This abstract class extends the ReceiveDescriptor
 * interface.
 *
 * Error- and status-handling will be archived due usage of the
 * interfaces ReceiveErrors and ReceiveStatus wrapping the fields of the descriptor.
 */
class RdDefault : public ReceiveDescriptor {
public:
    ~RdDefault() override = default;

    RdDefault(RdDefault const &) = delete;
    RdDefault &operator=(RdDefault const &) = delete;

protected:
    /**
     * Constructor.
     * @param address virtual address of the descriptor.
     * @param packetBufferAddress virtual address of the receive packet buffer.
     * @param errors Field wrapping the error bits.
     * @param status Field wrapping the status bits.
     */
    explicit RdDefault(uint8_t *address, uint8_t *packetBufferAddress, ReceiveErrors *errors, ReceiveStatus *status);

    /**
     * The virtual address of this descriptor inside the descriptor-block
     * buffer.
     */
    volatile uint8_t *address;

    /**
     * The virtual address of the receive packet buffer.
     */
    volatile  uint8_t *packetBufferAddress;

    /**
     * Encapsulates the errors field of this descriptor.
     */
    ReceiveErrors *errors;

    /**
     * Encapsulates the status field of this descriptor.
     */
    ReceiveStatus *status;

    /**
     * Inherited methods from ReceiveDescriptor.
     * This methods are meant to be overridden and
     * implemented by extending this class.
     */

    uint16_t readPacketChecksum() override = 0;
    uint16_t readSpecial() override = 0;

    /**
     * Inherited methods from ReceiveDescriptor.
     * This methods are meant to be overridden and
     * implemented by this class.
     */

    void writeAddress(uint64_t address) final;
    volatile uint8_t * getPacketAddress() final;
    uint16_t readLength() final;
    bool isEndOfPacked() final;
    void clearStatus() final;
    bool hasErrors() final;
    bool done() final;
};


#endif //HHUOS_RDDEFAULT_H
