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

#include "IcBufferedDefault.h"

IcBufferedDefault::IcBufferedDefault(uint32_t *address) : address(address) {}

bool IcBufferedDefault::isTransmitDescriptorWrittenBack() {
    return processInterrupt(0u);
}

bool IcBufferedDefault::isTransmitQueueEmpty() {
    return processInterrupt(1u);
}

bool IcBufferedDefault::hasLinkStatusChanged() {
    return processInterrupt(2u);
}

bool IcBufferedDefault::isReceiveDescriptorMinimumThresholdReached() {
    return processInterrupt(4u);
}

bool IcBufferedDefault::isReceiverOverrun() {
    return processInterrupt(6u);
}

bool IcBufferedDefault::hasReceiveTimerInterrupt() {
    return processInterrupt(7u);
}

bool IcBufferedDefault::isMdioAccessCompleted() {
    return processInterrupt(9u);
}

uint32_t IcBufferedDefault::readAndClear() {
    interrupts = *address;
    return interrupts;
}

bool IcBufferedDefault::processInterrupt(uint8_t interruptNumber) {
    auto asserted = (bool) (interrupts & (1u << interruptNumber));

    if (asserted)
        setInterruptHandled(interruptNumber);

    return asserted;
}

void IcBufferedDefault::setInterruptHandled(uint8_t interruptNumber) {
    interrupts &= ~(1u << interruptNumber);
}

bool IcBufferedDefault::hasUnhandledInterrupts() {
    return (interrupts != 0);
}

uint32_t IcBufferedDefault::getInterrupts() {
    return interrupts;
}
