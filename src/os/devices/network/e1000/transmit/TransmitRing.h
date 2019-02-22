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

#ifndef HHUOS_TRANSMITRING_H
#define HHUOS_TRANSMITRING_H


#include <cstdint>
#include <kernel/log/Logger.h>

/**
 * This is an interface for initializing the descriptor ring
 * on the ethernet card, and sending packets.
 * It interfaces between the hardware ring and the host ring,
 * and keeps them synchronized.
 */
class TransmitRing {
public:
    virtual ~TransmitRing() = default;

    TransmitRing(TransmitRing const &) = delete;
    TransmitRing &operator=(TransmitRing const &) = delete;

    /**
     * Initializes all registers according to the descriptor ring
     * on the ethernet card, i.e.
     * Base(s), Head, Tail and Length registers.
     * Should be called, before enabling the transmit bit
     * in the transmit-control register. Otherwise there
     * can be undefined behaviour of the ring.
     */
    virtual void initialize() = 0;

    /**
     * Called from upper layers to give the driver
     * control over the packet to send it.
     * @param address The physical address of the packet to send.
     * @param length The length of the packet to send.
     */
    virtual void sendPacket(uint64_t *address, uint16_t length) = 0;

protected:
    TransmitRing() = default;
};


#endif //HHUOS_TRANSMITRING_H
