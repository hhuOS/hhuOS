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

#include "ErDefault.h"

ErDefault::ErDefault(Register *request) : request(request) {}

uint16_t ErDefault::readData(uint8_t address) {
    clearData();
    setAddress(address);
    setStart();
    manage();

    pollDataTransferred();

    return takeData();
}

void ErDefault::clearData() {
    request->set(0x0000u << 16u, 0xFFFFu << 16u);
}

void ErDefault::setStart() {
    request->decide(1u << 0u, true);
}

uint16_t ErDefault::takeData() {
    return (uint16_t) (request->readDirect() >> 16u);
}

void ErDefault::manage() {
    request->confirm();
}

