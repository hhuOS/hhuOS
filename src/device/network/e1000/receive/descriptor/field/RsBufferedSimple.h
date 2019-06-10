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

#ifndef HHUOS_RSBUFFEREDSIMPLE_H
#define HHUOS_RSBUFFEREDSIMPLE_H


#include <cstdint>
#include "ReceiveStatus.h"

/**
 * This class implements the ReceiveStatus interface.
 *
 * With this class one fetches an status field, reads via
 * methods its information and finally clears it in memory.
 */
class RsBufferedSimple final : public ReceiveStatus {
public:
    /**
     * Constructor. The attribute 'status' will
     * be initialized with 0.
     * @param address Virtual address of this field, which points
     * inside the corresponding receive descriptor block.
     */
    explicit RsBufferedSimple(uint8_t *address);
    ~RsBufferedSimple() override = default;

private:
    /**
     * Address of the status field in an descriptor buffer.
     * (address = Buffer-address + position in Buffer).
     */
    volatile uint8_t *address;

    /**
     * The current status read from the buffer.
     */
    uint8_t status;

    /**
     * Inherited method from ReceiveStatus.
     * This methods are meant to be overridden and
     * implemented by this class.
     */

    bool descriptorDone() final;
    bool endOfPacket() final;
    bool ignoreChecksumIndication() final;
    bool matchesVet() final;
    bool tcpChecksumCalculatedOnPacket() final;
    bool ipChecksumCalculatedOnPacket() final;
    bool passedInExactFilter() final;
    uint8_t loadFromMemory() final;
    void clearInMemory() final;
};


#endif //HHUOS_RSBUFFEREDSIMPLE_H
