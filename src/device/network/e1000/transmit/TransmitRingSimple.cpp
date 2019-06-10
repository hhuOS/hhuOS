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

#include "TransmitRingSimple.h"


TransmitRingSimple::TransmitRingSimple(
        Descriptors<TransmitDescriptor *> *descriptors, HardwareDescriptorRing *initialize) :
        descriptors(descriptors), ringInitialize(initialize){}

void TransmitRingSimple::initialize() {
    ringInitialize->initialize();
}

void TransmitRingSimple::sendPacket(uint64_t *physicalAddress, uint16_t length) {
    auto *descriptor = descriptors->current();

    descriptor->writeAddress((uint64_t) physicalAddress);
    descriptor->writeLength(length);
    descriptor->writeCommand();

    ringInitialize->updateTail();

    while(!descriptor->isDone());

    descriptors->setNext();
}


