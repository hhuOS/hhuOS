/*
 * Copyright (C) 2018-2022 Heinrich-Heine-Universitaet Duesseldorf,
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

#include "kernel/service/InterruptService.h"
#include "device/interrupt/Pic.h"
#include "kernel/log/Logger.h"
#include "filesystem/memory/StreamNode.h"
#include "kernel/service/FilesystemService.h"
#include "kernel/system/System.h"
#include "SerialPort.h"
#include "device/port/serial/SerialPort.h"
#include "filesystem/core/Filesystem.h"
#include "filesystem/memory/MemoryDriver.h"
#include "kernel/interrupt/InterruptDispatcher.h"
#include "lib/util/Exception.h"

namespace Kernel {
struct InterruptFrame;
}  // namespace Kernel

namespace Device {

Kernel::Logger SerialPort::log = Kernel::Logger::get("COM");

SerialPort::SerialPort(ComPort port, BaudRate dataRate) : Util::Stream::FilterInputStream(inputStream),
        port(port), dataRate(dataRate), dataRegister(port), interruptRegister(port + 1), fifoControlRegister(port + 2),
        lineControlRegister(port + 3), modemControlRegister(port + 4), lineStatusRegister(port + 5),
        modemStatusRegister(port + 6), scratchRegister(port + 7) {
    interruptRegister.writeByte(0x00);      // Disable all interrupts
    lineControlRegister.writeByte(0x80);    // Enable DLAB, so that the divisor can be set

    dataRegister.writeByte(static_cast<uint8_t>(static_cast<uint16_t>(dataRate) & 0x0f));      // Divisor low byte
    interruptRegister.writeByte(static_cast<uint8_t>(static_cast<uint16_t>(dataRate) >> 8));   // Divisor high byte

    lineControlRegister.writeByte(0x03);    // 8 bits per char, no parity, one stop bit
    fifoControlRegister.writeByte(0x07);    // Enable FIFO-buffers, Clear FIFO-buffers, Trigger interrupt after each byte
    modemControlRegister.writeByte(0x0b);   // Enable data lines

    outputStream.connect(inputStream);
}

bool SerialPort::checkPort(ComPort port) {
    IoPort scratchRegister(port + 7);

    for (uint8_t i = 0; i < 0xff; i++) {
        scratchRegister.writeByte(i);
        if (scratchRegister.readByte() != i) {
            return false;
        }
    }

    return true;
}

void SerialPort::initializePort(ComPort port) {
    if (!checkPort(port)) {
        return;
    }

    log.info("Serial port [%s] detected", portToString(port));

    auto *serialPort = new SerialPort(port);
    auto *streamNode = new Filesystem::Memory::StreamNode(Util::Memory::String(portToString(port)).toLowerCase(), serialPort, serialPort);

    auto &filesystem = Kernel::System::getService<Kernel::FilesystemService>().getFilesystem();
    auto &driver = filesystem.getVirtualDriver("/device");
    bool success = driver.addNode("/", streamNode);

    if (success) {
        serialPort->plugin();
    } else {
        log.error("%s: Failed to add node", portToString(port));
        delete streamNode;
    }
}

void SerialPort::initializeAvailablePorts() {
    initializePort(COM1);
    initializePort(COM2);
    initializePort(COM3);
    initializePort(COM4);
}

SerialPort::ComPort SerialPort::portFromString(const Util::Memory::String &portName) {
    const auto port = portName.toLowerCase();

    if (port == "com1") {
        return COM1;
    } else if (port == "com2") {
        return COM2;
    } else if (port == "com3") {
        return COM3;
    } else if (port == "com4") {
        return COM4;
    } else {
        Util::Exception::throwException(Util::Exception::INVALID_ARGUMENT, "COM: Invalid port!");
    }
}

const char* SerialPort::portToString(const ComPort port) {
    switch (port) {
        case COM1:
            return "COM1";
        case COM2:
            return "COM2";
        case COM3:
            return "COM3";
        case COM4:
            return "COM4";
        default:
            Util::Exception::throwException(Util::Exception::INVALID_ARGUMENT, "COM: Invalid port!");
    }
}

void SerialPort::plugin() {
    auto &interruptService = Kernel::System::getService<Kernel::InterruptService>();
    if (port == COM1 || port == COM3) {
        interruptService.assignInterrupt(Kernel::InterruptDispatcher::COM1, *this);
        interruptService.allowHardwareInterrupt(Pic::Interrupt::COM1);
    } else {
        Kernel::System::getService<Kernel::InterruptService>().assignInterrupt(Kernel::InterruptDispatcher::COM2, *this);
        interruptService.allowHardwareInterrupt(Pic::Interrupt::COM2);
    }

    interruptRegister.writeByte(0x01);
}

void SerialPort::trigger(const Kernel::InterruptFrame &frame) {
    if ((fifoControlRegister.readByte() & 0x01) == 0x01) {
        return;
    }

    bool hasData = (lineStatusRegister.readByte() & 0x01) == 0x01;
    while (hasData) {
        uint8_t byte = dataRegister.readByte();
        outputStream.write(byte == 13 ? '\n' : byte);
        write(byte == 13 ? '\n' : byte);

        hasData = (lineStatusRegister.readByte() & 0x01) == 0x01;
    }
}

void SerialPort::setDataRate(SerialPort::BaudRate rate) {
    dataRate = rate;

    uint8_t interruptBackup = interruptRegister.readByte();
    uint8_t lineControlBackup = lineStatusRegister.readByte();

    interruptRegister.writeByte(0x00);                   // Disable all interrupts
    lineControlRegister.writeByte(0x80);                 // Enable to DLAB, so that the divisor can be set

    dataRegister.writeByte(static_cast<uint8_t>(static_cast<uint16_t>(rate) & 0x0f));       // Divisor low byte
    interruptRegister.writeByte(static_cast<uint8_t>(static_cast<uint16_t>(rate) >> 8));    // Divisor high byte

    lineControlRegister.writeByte(lineControlBackup);   // Restore line control register
    interruptRegister.writeByte(interruptBackup);       // Restore interrupt register
}

SerialPort::BaudRate SerialPort::getDataRate() const {
    return dataRate;
}

void SerialPort::write(uint8_t c) {
    if (c == '\n') {
        write(13);
    }

    while ((lineStatusRegister.readByte() & 0x20) == 0);
    dataRegister.writeByte(c);
}

void SerialPort::write(const uint8_t *sourceBuffer, uint32_t offset, uint32_t length) {
    for (uint32_t i = 0; i < length; i++) {
        write(sourceBuffer[offset + i]);
    }
}

}