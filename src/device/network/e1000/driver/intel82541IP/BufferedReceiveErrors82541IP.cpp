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

#include "BufferedReceiveErrors82541IP.h"

BufferedReceiveErrors82541IP::BufferedReceiveErrors82541IP(uint8_t *address) : ReBufferedDefault(address) {}

bool BufferedReceiveErrors82541IP::symbolError() {
    return (bool) (errors & (1u << 1u));
}

bool BufferedReceiveErrors82541IP::sequenceError() {
    return (bool) (errors & (1u << 2u));
}

bool BufferedReceiveErrors82541IP::carrierExtensionError() {
    Cpu::throwException(Cpu::Exception::NULLPOINTER, "Unsupported-Method call");
    return false;
}

bool BufferedReceiveErrors82541IP::rxDataError() {
    return (bool) (errors & (1u << 7u));
}

bool BufferedReceiveErrors82541IP::hasErrors() {
    return crcOrAlignmentError()
           || symbolError()
           || sequenceError()
           || tcpUdpChecksumError()
           || ipChecksumError()
           || rxDataError();
}
