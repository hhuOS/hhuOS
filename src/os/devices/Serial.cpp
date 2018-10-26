/*
 * Copyright (C) 2018 Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schoettner
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
 */

#include <kernel/interrupts/Pic.h>
#include <kernel/interrupts/IntDispatcher.h>
#include <lib/libc/printf.h>
#include <kernel/Kernel.h>
#include <kernel/events/input/SerialEvent.h>
#include "Serial.h"

uint16_t Serial::getBasePort(ComPort port) {
    auto *address = reinterpret_cast<uint16_t *>(0xc0000400);

    address += port - 1;

    return *address;
}

bool Serial::checkPort(ComPort port) {
    return getBasePort(port) != 0;
}

Serial::Serial(ComPort port, BaudRate speed) : eventBuffer(1024), port(port), speed(speed),
                                               dataRegister(getBasePort(port)),
                                               interruptRegister(static_cast<uint16_t>(getBasePort(port) + 1)),
                                               fifoControlRegister(static_cast<uint16_t>(getBasePort(port) + 2)),
                                               lineControlRegister(static_cast<uint16_t>(getBasePort(port) + 3)),
                                               modemControlRegister(static_cast<uint16_t>(getBasePort(port) + 4)),
                                               lineStatusRegister(static_cast<uint16_t>(getBasePort(port) + 5)),
                                               modemStatusRegister(static_cast<uint16_t>(getBasePort(port) + 6)),
                                               scratchRegister(static_cast<uint16_t>(getBasePort(port) + 7))
{
    interruptRegister.outb(0x00);        // Disable all interrupts
    lineControlRegister.outb(0x80);      // Enable DLAB, so that the divisor can be set

    dataRegister.outb(static_cast<uint8_t>(static_cast<uint16_t>(speed) & 0x0f));       // Divisor low byte
    interruptRegister.outb(static_cast<uint8_t>(static_cast<uint16_t>(speed) >> 8));    // Divisor high byte

    lineControlRegister.outb(0x03);      // 8 bits per char, no parity, one stop bit
    fifoControlRegister.outb(0xc7);      // Enable FIFO-buffers, Clear FIFO-buffers, Trigger interrupt after 14 bytes
    modemControlRegister.outb(0x0b);     // Enable data lines
}

char Serial::readChar() {
    while ((lineStatusRegister.inb() & 0x1u) == 0);

    return dataRegister.inb();
}

void Serial::sendChar(char c) {
    while ((lineStatusRegister.inb() & 0x20u) == 0);

    if(c == '\n') {
        dataRegister.outb(13);
    }

    dataRegister.outb(static_cast<uint8_t>(c));
}

void Serial::sendData(char *data, uint32_t len) {
    for(uint32_t i = 0; i < len; i++) {
        sendChar(data[i]);
    }
}

void Serial::readData(char *data, uint32_t len) {
    for(uint32_t i = 0; i < len; i++) {
        data[i] = readChar();
    }
}

void Serial::plugin() {
    eventBus = Kernel::getService<EventBus>();

    if(port == COM1 || port == COM3) {
        IntDispatcher::getInstance().assign(36, *this);
        Pic::getInstance()->allow(Pic::Interrupt::COM1);
    } else {
        IntDispatcher::getInstance().assign(35, *this);
        Pic::getInstance()->allow(Pic::Interrupt::COM2);
    }

    interruptRegister.outb(0x01);
}

void Serial::trigger(InterruptFrame &frame) {
    if((fifoControlRegister.inb() & 0x01u) == 1) {
        return;
    }

    bool hasData;

    do {
        hasData = (lineStatusRegister.inb() & 0x01u) == 0x01;

        if(hasData) {
            char c = dataRegister.inb();

            eventBuffer.push(SerialEvent(port, c));

            SerialEvent &event = eventBuffer.pop();

            eventBus->publish(event);
        }
    } while (hasData);
}

bool Serial::checkForData() {
    return (lineStatusRegister.inb() & 0x01u) == 0x01;
}

void Serial::setSpeed(BaudRate speed) {
    this->speed = speed;

    uint8_t interruptBackup = interruptRegister.inb();
    uint8_t lineControlBackup = lineStatusRegister.inb();

    interruptRegister.outb(0x00);                   // Disable all interrupts
    lineControlRegister.outb(0x80);                 // Enable to DLAB, so that the divisor can be set

    dataRegister.outb(static_cast<uint8_t>(static_cast<uint16_t>(speed) & 0x0f));       // Divisor low byte
    interruptRegister.outb(static_cast<uint8_t>(static_cast<uint16_t>(speed) >> 8));    // Divisor high byte

    lineControlRegister.outb(lineControlBackup);   // Restore line control register
    interruptRegister.outb(interruptBackup);       // Restore interrupt register
}

Serial::BaudRate Serial::getSpeed() {
    return speed;
}

Serial::ComPort Serial::getPortNumber() {
    return port;
}
