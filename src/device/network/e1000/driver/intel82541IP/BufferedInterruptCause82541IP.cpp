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

#include "device/cpu/Cpu.h"
#include "BufferedInterruptCause82541IP.h"

BufferedInterruptCause82541IP::BufferedInterruptCause82541IP(uint32_t *address) : IcBufferedDefault(address) {}

uint8_t BufferedInterruptCause82541IP::generalPurposeInterrupts_1() {
    Cpu::throwException(Cpu::Exception::NULLPOINTER, "Unsupported-Method call");
    return 0;
}

bool BufferedInterruptCause82541IP::hasReceiveSequenceError() {
    Cpu::throwException(Cpu::Exception::NULLPOINTER, "Unsupported-Method call");
    return false;
}

bool BufferedInterruptCause82541IP::hasReceivingCOrderedSets() {
    Cpu::throwException(Cpu::Exception::NULLPOINTER, "Unsupported-Method call");
    return false;
}

bool BufferedInterruptCause82541IP::isPhyInterrupt() {
    Cpu::throwException(Cpu::Exception::NULLPOINTER, "Unsupported-Method call");
    return false;
}

bool BufferedInterruptCause82541IP::isGeneralPurposeInterruptOnSDP6_2() {
    return processInterrupt(13u);
}

bool BufferedInterruptCause82541IP::isGeneralPurposeInterruptOnSDP7_3() {
    return processInterrupt(14u);
}

bool BufferedInterruptCause82541IP::isTransmitDescriptorLowThresholdHit() {
    return processInterrupt(15u);
}

bool BufferedInterruptCause82541IP::hasSmallReceivePacketDetected() {
    return processInterrupt(16u);
}
