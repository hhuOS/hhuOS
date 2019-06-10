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

#ifndef HHUOS_TDDEFAULT_H
#define HHUOS_TDDEFAULT_H

#include "field/TransmitStatus.h"
#include "field/LegacyCommand.h"
#include "TransmitDescriptor.h"

/**
 * This abstract class extends the TransmitDescriptor
 * interface.
 *
 * Command- and status-handling will be archived due usage of the
 * interfaces LegacyCommand and LegacyStatus wrapping the fields of the descriptor.
 */
class TdDefault : public TransmitDescriptor {
public:
    ~TdDefault() override = default;

    TdDefault(TdDefault const &) = delete;
    TdDefault &operator=(TdDefault const &) = delete;

protected:
    /**
     * Constructor.
     * @param address Virtual address of the descriptor.
     * @param command Field wrapping the command bits.
     * @param status Field wrapping the status bits.
     */
    explicit TdDefault(uint8_t *address, LegacyCommand *command, TransmitStatus *status);

    /**
     * The address of this descriptor inside the descriptor-block
     * buffer.
     */
    volatile uint8_t *address;

    /**
     * Encapsulates the command field of this descriptor.
     */
    LegacyCommand *command;

    /**
     * Encapsulates the status field of this descriptor.
     */
    TransmitStatus *status;

    /**
     * Inherited methods from ReceiveDescriptor.
     * This methods are meant to be overridden and
     * implemented by extending this class.
     */

    void writeCommand() override = 0;
    bool isDone() override = 0;

    /**
     * Inherited methods from ReceiveDescriptor.
     * This methods are meant to be overridden and
     * implemented by this class.
     */

    void writeAddress(uint64_t address) final;
    void writeLength(uint16_t length) final;
    void writeChecksumOffset(uint8_t offset) final;
    void writeChecksumStartField(uint8_t start) final;
    void writeSpecial(uint16_t value) final;
};


#endif //HHUOS_TDDEFAULT_H
