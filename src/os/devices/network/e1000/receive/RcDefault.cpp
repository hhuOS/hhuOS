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
 * All references marked with [...] refer to  following developers manual.
 * Intel Corporation. PCI/PCI-X Family of Gigabit Ethernet Controllers Software Developer’s Manual.
 * 317453006EN.PDF Revision 4.0. 2009
 */

#include "RcDefault.h"


RcDefault::RcDefault(Register *request) : request(request) {}

void RcDefault::receiver(bool enable) {
    request->decide(1u << 1u, enable);
}

void RcDefault::storeBadPackets(bool enable) {
    request->decide(1u << 2u, enable);
}

void RcDefault::unicastPromiscuous(bool enable) {
    request->decide(1u << 3u, enable);
}

void RcDefault::multicastPromiscuous(bool enable) {
    request->decide(1u << 4u, enable);
}

void RcDefault::longPacketReception(bool enable) {
    request->decide(1u << 5u, enable);
}

void RcDefault::descriptorMinimumThresholdSize(uint8_t value) {
    if (value > 2u) {
        Cpu::throwException(Cpu::Exception::INVALID_ARGUMENT, "Receive Descriptor Minimum Threshold Size is too high");
    }
    request->set(value << 8u, 3u << 8u);
}

void RcDefault::multicastOffset(uint8_t value) {
    if (value > 2u) {
        Cpu::throwException(Cpu::Exception::INVALID_ARGUMENT, "Multicast Offset is too high");
    }
    request->set(value << 12u, 3u << 12u);
}

void RcDefault::broadCastAcceptMode(bool enable) {
    request->decide(1u << 15u, enable);
}


void RcDefault::bufferSize(uint16_t value) {
    uint8_t mapped = 0;

    switch (value) {
        case 256: mapped = 3; bufferSizeExtension(false); break;
        case 512: mapped = 2; bufferSizeExtension(false); break;
        case 1024: mapped = 1; bufferSizeExtension(false); break;
        case 2048: mapped = 0; bufferSizeExtension(false); break;
        case 4096: mapped = 3; bufferSizeExtension(true); break;
        case 8192: mapped = 2; bufferSizeExtension(true); break;
        case 16384: mapped = 1; bufferSizeExtension(true); break;
        default:
            Cpu::throwException(Cpu::Exception::INVALID_ARGUMENT, "Unsupported Size of Receive Buffer");
    }

    request->set(mapped << 16u, 3u << 16u);
}

void RcDefault::canonicalFormIndicator(bool enable) {
    request->decide(1u << 19u, enable);
}

void RcDefault::canonicalFormIndicatorBitValue(bool enable) {
    request->decide(1u << 10u, enable);
}

void RcDefault::discardPauseFrames(bool enable) {
    request->decide(1u << 22u, enable);
}

void RcDefault::passMacControlFrames(bool enable) {
    request->decide(1u << 23u, enable);
}

void RcDefault::bufferSizeExtension(bool enable) {
    request->decide(1u << 25u, enable);
}

void RcDefault::stripEthernetCrc(bool enable) {
    request->decide(1u << 26u, enable);
}

void RcDefault::manage() {
    request->confirm();
}
