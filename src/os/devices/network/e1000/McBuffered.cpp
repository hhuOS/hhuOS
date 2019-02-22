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

#include "McBuffered.h"


uint8_t *McBuffered::read(EepromRead *eeprom) {
    uint16_t macWord1 = eeprom->readData(0);
    uint16_t macWord2 = eeprom->readData(1);
    uint16_t macWord3 = eeprom->readData(2);

    mac[0] = (uint8_t) macWord1;
    mac[1] = (uint8_t) (macWord1 >> 0x8u);
    mac[2] = (uint8_t) macWord2;
    mac[3] = (uint8_t) (macWord2 >> 0x8u);
    mac[4] = (uint8_t) macWord3;
    mac[5] = (uint8_t) (macWord3 >> 0x8u);

    return mac;
}

uint8_t *McBuffered::read(uint8_t *mioBase) {
    uint8_t *macAddress = mioBase + 0x5400;

    for(int i = 0; i < 6; i++) {
        mac[i] = macAddress[i];
    }
    return mac;
}

void McBuffered::writeTo(uint8_t *buffer, uint32_t position) {
    for(int i = position; i < (int) position + 6; i++) {
        *(buffer + i) = mac[i - position];
    }
}

String McBuffered::getStringRepresentation() {
    return String::format("%02x:%02x:%02x:%02x:%02x:%02x", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5], mac[6]);
}






