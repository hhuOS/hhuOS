/*
 * Copyright (C) 2018-2025 Heinrich-Heine-Universitaet Duesseldorf,
 * Institute of Computer Science, Department Operating Systems
 * Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schoettner
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
 */

#include "IoPort.h"

namespace Device {

IoPort::IoPort(uint16_t a) noexcept: address(a) {

}

uint16_t IoPort::getAddress() const {
    return address;
}

void IoPort::writeByte(uint8_t value) const {
    asm volatile ("outb %0, %1" : : "a"(value), "d"(address));
}

void IoPort::writeByte(uint16_t offset, uint8_t value) const {
    asm volatile ("outb %0, %1" : : "a"(value), "d"(address + offset));
}

void IoPort::writeWord(uint16_t value) const {
    asm volatile ("outw %0, %1" : : "a"(value), "d"(address));
}

void IoPort::writeWord(uint16_t offset, uint16_t value) const {
    asm volatile ("outw %0, %1" : : "a"(value), "d"(static_cast<uint16_t>(address + offset)));
}

void IoPort::writeDoubleWord(uint32_t value) const {
    asm volatile ("outl %0, %1" : : "a"(value), "d"(address));
}

void IoPort::writeDoubleWord(uint16_t offset, uint32_t value) const {
    asm volatile ("outl %0, %1" : : "a"(value), "d"(address + offset));
}

uint8_t IoPort::readByte() const {
    uint8_t ret;

    asm volatile ("inb %1, %0"
    : "=a"(ret)
    : "d"(address));

    return ret;
}

uint8_t IoPort::readByte(uint16_t offset) const {
    uint8_t ret;
    uint16_t addr = address + offset;

    asm volatile ("inb %1, %0"
    : "=a"(ret)
    : "d"(addr));

    return ret;
}

uint16_t IoPort::readWord() const {
    uint16_t ret;

    asm volatile ("inw %1, %0"
    : "=a"(ret)
    : "d"(address));
    return ret;
}

uint16_t IoPort::readWord(uint16_t offset) const {
    uint16_t ret;
    uint16_t addr = address + offset;

    asm volatile ("inw %1, %0"
    : "=a"(ret)
    : "d"(addr));

    return ret;
}

uint32_t IoPort::readDoubleWord() const {
    uint32_t ret;

    asm volatile ("inl %1, %0"
    : "=a"(ret)
    : "d"(address));
    return ret;
}

uint32_t IoPort::readDoubleWord(uint16_t offset) const {
    uint32_t ret;
    uint16_t addr = address + offset;

    asm volatile ("inl %1, %0"
    : "=a"(ret)
    : "d"(addr));

    return ret;
}

}