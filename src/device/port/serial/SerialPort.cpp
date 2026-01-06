/*
 * Copyright (C) 2017-2026 Heinrich Heine University Düsseldorf,
 * Institute of Computer Science, Department Operating Systems
 * Main developers: Christian Gesse <christian.gesse@hhu.de>, Fabian Ruhland <ruhland@hhu.de>
 * Original development team: Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schöttner
 * This project has been supported by several students.
 * A full list of integrated student theses can be found here: https://github.com/hhuOS/hhuOS/wiki/Student-theses
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
#include "device/interrupt/InterruptRequest.h"
#include "kernel/log/Log.h"
#include "filesystem/memory/StreamNode.h"
#include "kernel/service/FilesystemService.h"
#include "SerialPort.h"
#include "filesystem/Filesystem.h"
#include "filesystem/memory/MemoryDriver.h"
#include "kernel/interrupt/InterruptVector.h"
#include "device/port/serial/SerialPort.h"
#include "device/cpu/IoPort.h"
#include "device/port/serial/SimpleSerialPort.h"
#include "kernel/service/Service.h"
#include "lib/util/base/String.h"

namespace Kernel {
struct InterruptFrame;
}  // namespace Kernel

namespace Device {

SerialPort::SerialPort(Serial::ComPort port, Serial::BaudRate dataRate) : Util::Io::FilterInputStream(inputStream), inputBuffer(BUFFER_SIZE), inputStream(inputBuffer), port(port, dataRate) {}

void SerialPort::initializePort(Serial::ComPort port) {
    if (!checkPort(port)) {
        return;
    }

    LOG_INFO("Serial port [%s] detected", portToString(port));

    auto *serialPort = new SerialPort(port);
    auto *streamNode = new Filesystem::Memory::StreamNode(Util::String(portToString(port)).toLowerCase(), serialPort, serialPort);

    auto &filesystem = Kernel::Service::getService<Kernel::FilesystemService>().getFilesystem();
    auto &driver = filesystem.getVirtualDriver("/device");
    bool success = driver.addNode("/", streamNode);

    if (success) {
        serialPort->plugin();
    } else {
        LOG_ERROR("%s: Failed to add node", portToString(port));
        delete streamNode;
    }
}

void SerialPort::initializeAvailablePorts() {
    initializePort(Serial::COM1);
    initializePort(Serial::COM2);
    initializePort(Serial::COM3);
    initializePort(Serial::COM4);
}

void SerialPort::plugin() {
    auto &interruptService = Kernel::Service::getService<Kernel::InterruptService>();
    if (port.getPort() == Serial::COM1 || port.getPort() == Serial::COM3) {
        interruptService.assignInterrupt(Kernel::InterruptVector::COM1, *this);
        interruptService.allowHardwareInterrupt(Device::InterruptRequest::COM1);
    } else {
        Kernel::Service::getService<Kernel::InterruptService>().assignInterrupt(Kernel::InterruptVector::COM2, *this);
        interruptService.allowHardwareInterrupt(Device::InterruptRequest::COM2);
    }

    port.interruptRegister.writeByte(0x01);
}

void SerialPort::trigger([[maybe_unused]] const Kernel::InterruptFrame &frame, [[maybe_unused]] Kernel::InterruptVector slot) {
    if (port.fifoControlRegister.readByte() & 0x01) {
        return;
    }

    while (port.lineStatusRegister.readByte() & 0x01) {
        uint8_t byte = port.dataRegister.readByte();
        inputBuffer.offer(byte == 13 ? '\n' : byte);
    }
}

void SerialPort::setDataRate(Serial::BaudRate rate) {
    port.setDataRate(rate);
}

Serial::BaudRate SerialPort::getDataRate() const {
    return port.getDataRate();
}

bool SerialPort::write(uint8_t c) {
    return port.write(c);
}

uint32_t SerialPort::write(const uint8_t *sourceBuffer, uint32_t offset, uint32_t length) {
    return port.write(sourceBuffer, offset, length);
}

uint8_t SerialPort::readDirect() {
    return port.read();
}

}