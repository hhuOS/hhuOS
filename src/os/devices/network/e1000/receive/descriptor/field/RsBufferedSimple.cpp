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

#include "RsBufferedSimple.h"

RsBufferedSimple::RsBufferedSimple(uint8_t *address)
        : address(address), status(0) {}

bool RsBufferedSimple::descriptorDone() {
    return (bool) (status & (1u << 0u));
}

bool RsBufferedSimple::endOfPacket() {
    return (bool) (status & (1u << 1u));
}

bool RsBufferedSimple::ignoreChecksumIndication() {
    return (bool) (status & (1u << 2u));
}

bool RsBufferedSimple::matchesVet() {
    return (bool) (status & (1u << 3u));
}

bool RsBufferedSimple::tcpChecksumCalculatedOnPacket() {
    return (bool) (status & (1u << 5u));
}

bool RsBufferedSimple::ipChecksumCalculatedOnPacket() {
    return (bool) (status & (1u << 6u));
}

bool RsBufferedSimple::passedInExactFilter() {
    return (bool) (status & (1u << 7u));
}

uint8_t RsBufferedSimple::loadFromMemory() {
    status = *address;
    return status;
}

void RsBufferedSimple::clearInMemory() {
    *address = 0;
}


