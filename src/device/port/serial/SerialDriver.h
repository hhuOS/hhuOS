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
#include <kernel/log/Logger.h>
#include <kernel/service/PortService.h>
#include <filesystem/core/Filesystem.h>

#include "kernel/core/Kernel.h"
#include "device/cpu/IoPort.h"
#include "kernel/service/EventBus.h"
#include "lib/util/RingBuffer.h"
#include "device/misc/Pic.h"
#include "kernel/interrupt/InterruptDispatcher.h"
#include "device/port/Port.h"
#include "Common.h"
#include "SerialEvent.h"
#include "SerialNode.h"
#include "BaudRateNode.h"

namespace Serial {

/**
 * Driver for the serial COM-ports.
 *
 * @author Fabian Ruhland
 * @date 2018
 */
template<ComPort port>
class SerialDriver : public Port, public InterruptHandler {

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
     * Overriding function from InterruptHandler.
     */
    bool hasInterruptData() override;

    /**
     * Overriding function from InterruptHandler.
     */
    void parseInterruptData() override;

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

    EventBus *eventBus;

    Util::RingBuffer<uint8_t> interruptDataBuffer;

    BaudRate speed;

    IoPort dataRegister;
    IoPort interruptRegister;
    IoPort fifoControlRegister;
    IoPort lineControlRegister;
    IoPort modemControlRegister;
    IoPort lineStatusRegister;
    IoPort modemStatusRegister;
    IoPort scratchRegister;

    static const constexpr char *NAME_1 = "Com1";
    static const constexpr char *NAME_2 = "Com2";
    static const constexpr char *NAME_3 = "Com3";
    static const constexpr char *NAME_4 = "Com4";
};

template<ComPort port>
bool SerialDriver<port>::checkPort() {
    IoPort scratchRegister(port + 7);
    
    for(uint8_t i = 0; i < 0xff; i++) {
        scratchRegister.outb(i);
        
        if(scratchRegister.inb() != i) {
            return false;
        }
    }
    
    return true;
}

template<ComPort port>
SerialDriver<port>::SerialDriver(BaudRate speed) : interruptDataBuffer(1024), speed(speed),
                                                   dataRegister(port),
                                                   interruptRegister(port + 1),
                                                   fifoControlRegister(port + 2),
                                                   lineControlRegister(port + 3),
                                                   modemControlRegister(port + 4),
                                                   lineStatusRegister(port + 5),
                                                   modemStatusRegister(port + 6),
                                                   scratchRegister(port + 7) {
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
    InterruptManager::getInstance().registerInterruptHandler(this);

    if (port == COM1 || port == COM3) {
        InterruptDispatcher::getInstance().assign(36, *this);
        Pic::getInstance().allow(Pic::Interrupt::COM1);
    } else {
        InterruptDispatcher::getInstance().assign(35, *this);
        Pic::getInstance().allow(Pic::Interrupt::COM2);
    }

    interruptRegister.outb(0x01);
}

template<ComPort port>
void SerialDriver<port>::trigger(InterruptFrame &frame) {
    if ((fifoControlRegister.inb() & 0x01) == 0x01) {
        return;
    }

    bool hasData;

    do {
        hasData = (lineStatusRegister.inb() & 0x01u) == 0x01;

        if (hasData) {
            interruptDataBuffer.push(dataRegister.inb());
        }
    } while (hasData);
}

template<ComPort port>
bool SerialDriver<port>::hasInterruptData() {
    return !interruptDataBuffer.isEmpty();
}

template<ComPort port>
void SerialDriver<port>::parseInterruptData() {
    uint8_t data = interruptDataBuffer.pop();

    Util::SmartPointer<Event> event(new SerialEvent<port>(data));

    eventBus->publish(event);
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
    switch(port) {
        case COM1 :
            return NAME_1;
        case COM2 :
            return NAME_2;
        case COM3 :
            return NAME_3;
        case COM4 :
            return NAME_4;
    }
}

static void initializePorts() {

    Logger &log = Logger::get("SERIAL");

    auto *portService = Kernel::getService<PortService>();
    auto *fileSystem = Kernel::getService<Filesystem>();

    if(Serial::SerialDriver<Serial::COM1>::checkPort()) {
        log.info("Detected COM1");
        auto *com1 = new Serial::SerialDriver<Serial::COM1>;

        portService->registerPort(com1);

        fileSystem->createDirectory("/dev/ports/serial1");
        fileSystem->addVirtualNode("/dev/ports/serial1", new Serial::SerialNode<Serial::COM1>(com1));
        fileSystem->addVirtualNode("/dev/ports/serial1", new Serial::BaudRateNode<Serial::COM1>(com1));

        com1->plugin();
    }

    if(Serial::SerialDriver<Serial::COM2>::checkPort()) {
        log.info("Detected COM2");
        auto *com2 = new Serial::SerialDriver<Serial::COM2>;

        portService->registerPort(com2);

        fileSystem->createDirectory("/dev/ports/serial2");
        fileSystem->addVirtualNode("/dev/ports/serial2", new Serial::SerialNode<Serial::COM2>(com2));
        fileSystem->addVirtualNode("/dev/ports/serial2", new Serial::BaudRateNode<Serial::COM2>(com2));

        com2->plugin();
    }

    if(Serial::SerialDriver<Serial::COM3>::checkPort()) {
        log.info("Detected COM3");
        auto *com3 = new Serial::SerialDriver<Serial::COM3>;

        portService->registerPort(com3);

        fileSystem->createDirectory("/dev/ports/serial3");
        fileSystem->addVirtualNode("/dev/ports/serial3", new Serial::SerialNode<Serial::COM3>(com3));
        fileSystem->addVirtualNode("/dev/ports/serial3", new Serial::BaudRateNode<Serial::COM3>(com3));

        com3->plugin();
    }

    if(Serial::SerialDriver<Serial::COM4>::checkPort()) {
        log.info("Detected COM4");
        auto *com4 = new Serial::SerialDriver<Serial::COM4>;

        portService->registerPort(com4);

        fileSystem->createDirectory("/dev/ports/serial4");
        fileSystem->addVirtualNode("/dev/ports/serial4", new Serial::SerialNode<Serial::COM4>(com4));
        fileSystem->addVirtualNode("/dev/ports/serial4", new Serial::BaudRateNode<Serial::COM4>(com4));

        com4->plugin();
    }
}

}

#endif
