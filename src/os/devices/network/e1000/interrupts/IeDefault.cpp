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

#include "IeDefault.h"


IeDefault::IeDefault(Register *request) : request(request) {}

void IeDefault::transmitDescriptorWrittenBack() {
    request->decide(1u << 0u, true);
}

void IeDefault::transmitQueueEmpty() {
    request->decide(1u << 1u, true);
}

void IeDefault::linkStatusChange() {
    request->decide(1u << 2u, true);
}

void IeDefault::receiveDescriptorMinimumThresholdHit() {
    request->decide(1u << 4u, true);
}

void IeDefault::receiverFifoOverrun() {
    request->decide(1u << 6u, true);
}

void IeDefault::receiveTimer() {
    request->decide(1u << 7u, true);
}

void IeDefault::mdioAccessComplete() {
    request->decide(1u << 9u, true);
}

void IeDefault::generalPurposeInterrupts1(uint8_t value) {
    request->set(value << 13u, 0x11u << 13u);
}

void IeDefault::manage() {
    request->confirm();
}
