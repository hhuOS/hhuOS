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

#ifndef HHUOS_SERIALDRIVER_H
#define HHUOS_SERIALDRIVER_H

#include <cstdint>
#include <kernel/Kernel.h>
#include <kernel/IOport.h>
#include <kernel/services/EventBus.h>
#include <lib/util/RingBuffer.h>
#include <kernel/interrupts/Pic.h>
#include <kernel/interrupts/IntDispatcher.h>
#include "devices/IODevice.h"
#include "devices/ports/Port.h"
#include "Common.h"
#include "SerialEvent.h"

namespace Serial {

/**
 * Driver for the serial COM-ports.
 *
 * @author Fabian Ruhland
 * @date 2018
 */
template<ComPort port>
class SerialDriver : public Port, public IODevice {

public:

    /**
     * Check if a COM-port exists.
     * Always check if the COM-port exists before creating an instance of this class!
     *
     */
    static bool checkPort();

    /**
     * Constructor.
     *
     * @param speed The baud-rate
     */
    explicit SerialDriver(BaudRate speed = BaudRate::BAUD_115200);

    /**
     * Copy-constructor.
     */
    SerialDriver(const SerialDriver &other) = delete;

    /**
     * Destructor.
     */
    SerialDriver &operator=(const SerialDriver &other) = delete;

    /**
     * Destructor.
     */
    ~SerialDriver() override = default;

    /**
     * Enable interrupts for this COM-port.
     */
    void plugin();

    /**
     * Overriding function from IODevice.
     */
    void trigger(InterruptFrame &frame) override;

    /**
     * Overriding function from Port.
     */
    char readChar() override;

    /**
     * Overriding function from Port.
     */
    void sendChar(char c) override;

    /**
     * Overriding function from Port.
     */
    String getName() override;

    /**
     * Overriding function from IODevice.
     */
    bool checkForData() override;

    /**
     * Set the baud-rate.
     *
     * @param speed The baud-rate
     */
    void setSpeed(BaudRate speed);

    /**
     * Get the baud-rate.
     */
    BaudRate getSpeed();

private:

    /**
     * Calculate the base address of the IO-ports of a given parallel port.
     *
     * @param port The port
     * @return The base address
     */
    static uint16_t getBasePort();

private:

    BaudRate speed;

    EventBus *eventBus;
    Util::RingBuffer<SerialEvent<port>> eventBuffer;

    IOport dataRegister;
    IOport interruptRegister;
    IOport fifoControlRegister;
    IOport lineControlRegister;
    IOport modemControlRegister;
    IOport lineStatusRegister;
    IOport modemStatusRegister;
    IOport scratchRegister;
};

template<ComPort port>
uint16_t SerialDriver<port>::getBasePort() {
    auto *address = reinterpret_cast<uint16_t *>(0xc0000400);

    address += port - 1;

    return *address;
}

template<ComPort port>
bool SerialDriver<port>::checkPort() {
    return getBasePort() != 0;
}

template<ComPort port>
SerialDriver<port>::SerialDriver(BaudRate speed) : eventBuffer(1024), speed(speed),
                                                   dataRegister(getBasePort()),
                                                   interruptRegister(static_cast<uint16_t>(getBasePort() + 1)),
                                                   fifoControlRegister(static_cast<uint16_t>(getBasePort() + 2)),
                                                   lineControlRegister(static_cast<uint16_t>(getBasePort() + 3)),
                                                   modemControlRegister(static_cast<uint16_t>(getBasePort() + 4)),
                                                   lineStatusRegister(static_cast<uint16_t>(getBasePort() + 5)),
                                                   modemStatusRegister(static_cast<uint16_t>(getBasePort() + 6)),
                                                   scratchRegister(static_cast<uint16_t>(getBasePort() + 7)) {
    eventBus = Kernel::getService<EventBus>();

    interruptRegister.outb(0x00);        // Disable all interrupts
    lineControlRegister.outb(0x80);      // Enable DLAB, so that the divisor can be set

    dataRegister.outb(static_cast<uint8_t>(static_cast<uint16_t>(speed) & 0x0f));       // Divisor low byte
    interruptRegister.outb(static_cast<uint8_t>(static_cast<uint16_t>(speed) >> 8));    // Divisor high byte

    lineControlRegister.outb(0x03);      // 8 bits per char, no parity, one stop bit
    fifoControlRegister.outb(0xc7);      // Enable FIFO-buffers, Clear FIFO-buffers, Trigger interrupt after 14 bytes
    modemControlRegister.outb(0x0b);     // Enable data lines
}

template<ComPort port>
char SerialDriver<port>::readChar() {
    while ((lineStatusRegister.inb() & 0x1u) == 0);

    return dataRegister.inb();
}

template<ComPort port>
void SerialDriver<port>::sendChar(char c) {
    while ((lineStatusRegister.inb() & 0x20u) == 0);

    if (c == '\n') {
        dataRegister.outb(13);
    }

    dataRegister.outb(static_cast<uint8_t>(c));
}

template<ComPort port>
void SerialDriver<port>::plugin() {
    if (port == COM1 || port == COM3) {
        IntDispatcher::getInstance().assign(36, *this);
        Pic::getInstance().allow(Pic::Interrupt::COM1);
    } else {
        IntDispatcher::getInstance().assign(35, *this);
        Pic::getInstance().allow(Pic::Interrupt::COM2);
    }

    interruptRegister.outb(0x01);
}

template<ComPort port>
void SerialDriver<port>::trigger(InterruptFrame &frame) {
    if ((fifoControlRegister.inb() & 0x01u) == 1) {
        return;
    }

    bool hasData;

    do {
        hasData = (lineStatusRegister.inb() & 0x01u) == 0x01;

        if (hasData) {
            char c = dataRegister.inb();

            eventBuffer.push(SerialEvent<port>(c));

            SerialEvent<port> &event = eventBuffer.pop();

            eventBus->publish(event);
        }
    } while (hasData);
}

template<ComPort port>
bool SerialDriver<port>::checkForData() {
    return (lineStatusRegister.inb() & 0x01u) == 0x01;
}

template<ComPort port>
void SerialDriver<port>::setSpeed(BaudRate speed) {
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

template<ComPort port>
BaudRate SerialDriver<port>::getSpeed() {
    return speed;
}

template<ComPort port>
String SerialDriver<port>::getName() {
    return "COM" + String::valueOf(port, 10);
}

}

#endif
