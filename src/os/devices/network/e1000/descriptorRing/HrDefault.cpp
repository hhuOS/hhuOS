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

#include "HrDefault.h"

HrDefault::HrDefault(uint8_t *virtualBase, uint64_t physicalAddress, uint16_t descriptors) : virtualBase(
        virtualBase), physicalAddress(physicalAddress), descriptors(descriptors), tail(0) {}


uint32_t *HrDefault::chooseRegister(uint8_t number) {
    return (uint32_t *) (virtualBase + number);
}

void HrDefault::initBase() {
    *chooseRegister(0x00) = (uint32_t) (physicalAddress & 0xFFFFFFFFu);
    *chooseRegister(0x04) = (uint32_t) (physicalAddress >> 0x20u);
}

void HrDefault::initLength() {
    *chooseRegister(0x08) = descriptors * 16u;
}

void HrDefault::initHead() {
    *chooseRegister(0x10) = 0;
}


void HrDefault::initialize() {
    initBase();
    initLength();
    initHead();
    initTail();
}

void HrDefault::updateTail() {
    tail = (uint16_t) ((tail + 1u) % descriptors);
    *chooseRegister(0x18) = tail;
}


