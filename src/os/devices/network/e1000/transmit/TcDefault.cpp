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

#include "TcDefault.h"

TcDefault::TcDefault(Register *request) : request(request) {}

void TcDefault::enableTransmit(bool enable) {
    request->decide(1u << 1u, enable);
}

void TcDefault::padShortPackets(bool enable) {
    request->decide(1u << 3u, enable);
}

void TcDefault::collisionThreshold(uint8_t value) {
    request->set(value << 4u, 0xFFu << 4u);
}

void TcDefault::collisionDistance(uint16_t value) {
    request->set(value << 12u, 0x3FFu << 12u);
}

void TcDefault::softwareXOffTransmission(bool enable) {
    request->decide(1u << 22u, enable);
}

void TcDefault::retransmitOnLateCollision(bool enable) {
    request->decide(1u << 24u, enable);
}

void TcDefault::manage() {
    request->confirm();
}
