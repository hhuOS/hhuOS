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

#ifndef HHUOS_RECEIVERING_H
#define HHUOS_RECEIVERING_H

#include <kernel/log/Logger.h>

/**
 * This is an interface for initializing the descriptor ring
 * on the ethernet card, and fetching incoming packets.
 * It interfaces between the hardware ring and the host ring,
 * and keeps them synchronized.
 */
class ReceiveRing {
public:
    virtual ~ReceiveRing() = default;

    ReceiveRing(ReceiveRing const &) = delete;
    ReceiveRing &operator=(ReceiveRing const &) = delete;

    /**
     * Initializes all registers according to the descriptor ring
     * on the ethernet card, i.e.
     * Base(s), Head, Tail and Length registers.
     * Should be called, before enabling the receive bit
     * in the receive-control register. Otherwise there
     * can be undefined behaviour of the ring.
     */
    virtual void initialize() = 0;

    /**
     * Progresses pending packets (already handled by hardware) and calls an
     * registered class for handling the received packet.
     * Called when the receiver timer expires.
     * @param log For displaying receive packet relevant exceptional information.
     * @param mioBase Base address of the MMIO-space.
     */
    virtual void receivePoll(uint8_t *mioBase) = 0;

protected:
    ReceiveRing() = default;
};


#endif //HHUOS_RECEIVERING_H
