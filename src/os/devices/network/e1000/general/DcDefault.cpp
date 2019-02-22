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

#include "DcDefault.h"

DcDefault::DcDefault(Register *request) : request(request) {}

void DcDefault::fullDuplex(bool enable) {
    request->decide(1u << 0u, enable);
}

void DcDefault::autoSpeedDetection(bool enable) {
    request->decide(1u << 5u, enable);
}

void DcDefault::setLinkUp(bool enable) {
    request->decide(1u << 6u, enable);
}

void DcDefault::speedSelection(uint8_t value) {
    request->set(value << 8u, 0x11u << 8u);
}

void DcDefault::forceSpeed(bool enable) {
    request->decide(1u << 11u, enable);
}

void DcDefault::forceDuplex(bool enable) {
    request->decide(1u << 12u, enable);
}

void DcDefault::sdp0Data(bool mode) {
    request->decide(1u << 18u, mode);
}

void DcDefault::sdp1Data(bool mode) {
    request->decide(1u << 19u, mode);
}

void DcDefault::phyPowerManagement(bool enable) {
    request->decide(1u << 21u, enable);
}

void DcDefault::sdp0IoDirection(bool direction) {
    request->decide(1u << 22u, direction);
}

void DcDefault::sdp1IoDirection(bool direction) {
    request->decide(1u << 23u, direction);
}

void DcDefault::deviceReset(bool enable) {
    request->decide(1u << 26u, enable);
}

void DcDefault::flowControlReceive(bool enable) {
    request->decide(1u << 27u, enable);
}

void DcDefault::flowControlTransmit(bool enable) {
    request->decide(1u << 28u, enable);
}

void DcDefault::manage() {
    request->confirm();
}







