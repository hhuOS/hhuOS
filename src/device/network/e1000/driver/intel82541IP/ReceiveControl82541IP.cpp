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

#include "ReceiveControl82541IP.h"

ReceiveControl82541IP::ReceiveControl82541IP(Register *request) : RcDefault(request) {}

void ReceiveControl82541IP::loopbackMode(uint8_t value) {
    if ((value > 3u) | (value == 2u) | (value == 1u)) {
        Cpu::throwException(Cpu::Exception::INVALID_ARGUMENT, "Inappropriate loopback Mode");
    }
    request->set(value << 6u, 3u << 6u);
}

void ReceiveControl82541IP::vlanFilter(bool enable) {
    request->decide(1u << 18u, enable);
}
