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

#include "RdDefault.h"

RdDefault::RdDefault(uint8_t *address, uint8_t *packetBufferAddress, ReceiveErrors *errors, ReceiveStatus *status)
        : address(address), packetBufferAddress(packetBufferAddress), errors(errors), status(status) {}

void RdDefault::writeAddress(uint64_t address) {
    *((uint64_t *) this->address) = address;
}

volatile uint8_t * RdDefault::getPacketAddress() {
    return packetBufferAddress;
}

uint16_t RdDefault::readLength() {
    return *((uint16_t *) (address + 8));
}

bool RdDefault::isEndOfPacked() {
    return status->endOfPacket();
}

void RdDefault::clearStatus() {
    status->clearInMemory();
}

bool RdDefault::hasErrors() {
    return errors->hasErrors();
}


bool RdDefault::done() {
    status->loadFromMemory();
    return status->descriptorDone();
}
