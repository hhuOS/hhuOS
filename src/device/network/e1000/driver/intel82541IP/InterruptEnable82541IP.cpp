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

#include "InterruptEnable82541IP.h"

InterruptEnable82541IP::InterruptEnable82541IP(Register *request) : IeDefault(request) {}

void InterruptEnable82541IP::receiveSequenceError() {
    Cpu::throwException(Cpu::Exception::NULLPOINTER, "Unsupported-Method call");
}

void InterruptEnable82541IP::receivingCOrderedSets() {
    Cpu::throwException(Cpu::Exception::NULLPOINTER, "Unsupported-Method call");
}

void InterruptEnable82541IP::phyInterrupt() {
    Cpu::throwException(Cpu::Exception::NULLPOINTER, "Unsupported-Method call");
}

void InterruptEnable82541IP::generalPurposeInterrupts2(uint8_t value) {
    Cpu::throwException(Cpu::Exception::NULLPOINTER, "Unsupported-Method call");
}

void InterruptEnable82541IP::transmitDescriptorLowThresholdHit() {
    request->decide(1u << 15u, true);
}

void InterruptEnable82541IP::smallReceivePacketDetection() {
    request->decide(1u << 16u, true);
}