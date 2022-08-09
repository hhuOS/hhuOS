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

#include "Mouse.h"
#include "kernel/system/System.h"
#include "kernel/service/InterruptService.h"
#include "filesystem/memory/StreamNode.h"
#include "kernel/service/FilesystemService.h"
#include "lib/util/async/Thread.h"

namespace Device {

Kernel::Logger Mouse::log = Kernel::Logger::get("Mouse");

Mouse::Mouse() {
    outputStream.connect(inputStream);
}

void Mouse::initialize() {
    auto *mouse = new Mouse();

    // Check if mouse is available
    mouse->waitControl();
    mouse->controlPort.writeByte(0xA9); // Test second PS2 port (works only if two PS2 ports are supported)
    mouse->waitData();
    auto data = mouse->dataPort.readByte();
    if (data == 0xff) {
        log.error("No secondary PS2 port available");
        delete mouse;
        return;
    }

    log.info("Found secondary PS2 port -> Activating mouse");

    // Deactivate keyboard in ps2 status byte
    mouse->waitControl();
    mouse->controlPort.writeByte(0x20); // Read status byte from controller
    mouse->waitData();
    auto status = mouse->dataPort.readByte() & 0xfc; // Deactivate mouse and keyboard interrupts in controller
    status |= 0x10; // Deactivate keyboard too (1 means disabled)
    mouse->waitControl();

    // Write modified byte back to controller
    mouse->controlPort.writeByte(0x60);
    mouse->waitControl();
    mouse->dataPort.writeByte(status);

    // Activate auxiliary device
    mouse->waitControl();
    mouse->controlPort.writeByte(0xA8); // A8 - enables second ps2 port

    // Activate mouse and keyboard interrupts
    mouse->waitControl();
    mouse->controlPort.writeByte(0x20); // Read status byte from controller
    mouse->waitData();
    status = mouse->dataPort.readByte() | 0x03; // Activate mouse and keyboard interrupts (first two bits in status byte)
    status &= ~0x10; // Activate keyboard
    mouse->waitControl();
    mouse->controlPort.writeByte(0x60); // Write modified byte back to controller
    mouse->waitControl();
    mouse->dataPort.writeByte(status);

    // Use mouse default settings
    if (!mouse->writeCommand(0xF6)) {
        mouse->cleanup();
        delete mouse;
        return;
    }
    // Set resolution
    if (!mouse->writeCommandAndByte(0xE8, 0x02)) {
        mouse->cleanup();
        delete mouse;
        return;
    }
    // Set sampling to 80 packets per second
    if (!mouse->writeCommandAndByte(0xF3, 80)) {
        mouse->cleanup();
        delete mouse;
        return;
    }
    // Activate mouse packet streaming
    if (!mouse->writeCommand(0xF4)) {
        mouse->cleanup();
        delete mouse;
        return;
    }

    auto *streamNode = new Filesystem::Memory::StreamNode("mouse", mouse);
    auto &filesystem = Kernel::System::getService<Kernel::FilesystemService>().getFilesystem();
    auto &driver = filesystem.getVirtualDriver("/device");
    bool success = driver.addNode("/", streamNode);

    if (success) {
        mouse->plugin();
    } else {
        log.error("Mouse: Failed to add node");
        mouse->cleanup();
        delete streamNode;
    }
}

void Mouse::plugin() {
    auto &interruptService = Kernel::System::getService<Kernel::InterruptService>();
    interruptService.assignInterrupt(Kernel::InterruptDispatcher::MOUSE, *this);
    interruptService.allowHardwareInterrupt(Pic::Interrupt::MOUSE);
}

void Mouse::trigger(const Kernel::InterruptFrame &frame) {
    auto status = controlPort.readByte();
    if (!(status & 0x20)) {
        return;
    }
    
    auto data = dataPort.readByte();
    switch (cycle) {
        case 1:
            flags = data;
            // Check if the always 1 bit is set (hacky way to check if it is really the first cycle)
            if(!(flags & 0x08)) {
                cycle = 1;
            } else {
                cycle++;
            }

            break;
        case 2:
            // Just get the correct bits
            data &= 0xff;
            // Check if signed
            if(flags & 0x10) {
                // Extend unsigned 8 bit data value to unsigned 32-bit in twos complement
                dy = static_cast<int32_t>(data | 0xffffff00);
            } else {
                dy = data;
            }

            cycle++;
            break;
        case 3:
            // Just get the correct bits
            data &= 0xff;
            // Check if signed
            if (flags & 0x20) {
                // Extend unsigned 8 bit data value to unsigned 32-bit in twos complement
                dx = static_cast<int32_t>(data | 0xffffff00);
            } else {
                dx = data;
            }

            // If there was an x or y overflow, discard this 'event'
            if (flags & 0x40 || flags & 0x80){
                return;
            }

            // Write data: 1. button mask, 2. relative x-movement, 3. relative y-movement
            outputStream.write(flags & 0x07);
            outputStream.write(dx);
            outputStream.write(dy);

            // Reset cycle
            cycle = 1;
            break;
    }
}

void Mouse::waitData() {
    uint32_t timeout = 0;
    while ((controlPort.readByte() & 0x01) && timeout < TIMEOUT) {
        Util::Async::Thread::sleep(Util::Time::Timestamp::ofMilliseconds(10));
        timeout += 10;
    }
}

void Mouse::waitControl() {
    uint32_t timeout = 0;
    while ((controlPort.readByte() & 0x02) && timeout < TIMEOUT) {
        Util::Async::Thread::sleep(Util::Time::Timestamp::ofMilliseconds(10));
        timeout += 10;
    }
}

uint8_t Mouse::readByte() {
    waitData();
    return dataPort.readByte();
}

void Mouse::writeByte(uint8_t data) {
    waitControl();
    controlPort.writeByte(0xD4); // Write next data byte to second PS2 input buffer (in this case mouse)
    waitControl(); // Wait for OK
    dataPort.writeByte(data);
}

bool Mouse::writeCommand(uint8_t command) {
    uint8_t tmp;
    uint8_t counter = 0;
    writeByte(command); // Send command

    while((tmp = readByte()) != 0xfa) { // Wait for ACK
        log.warn("Did not receive ACK");
        if(tmp == 0xFE) {
            writeByte(command); // send command byte again
        }

        counter ++;
        if(counter == 5) {
            log.error("Did not receive ACK for after 5 retries");
            return false;
        }
    }

    return true;
}

bool Mouse::writeCommandAndByte(uint8_t command, uint8_t data) {
    if (!writeCommand(command)) {
        return false;
    }

    uint8_t tmp;
    writeByte(data); // Write payload for command to data port

    while((tmp = readByte()) != 0xfa){ // wait for ack
        if(tmp == 0xfe) {
            writeByte(data);
        }
    }

    return true;
}

void Mouse::cleanup() {
    uint8_t status;

    waitControl();
    controlPort.writeByte(0x20); // Read controller status byte
    waitData();
    status = dataPort.readByte() & 0xfc; // Deactivate mouse and keyboard interrupts
    status |= 0x10; // Disable keyboard

    waitControl();
    controlPort.writeByte(0x60); // Write modified status byte back to controller
    waitControl();
    dataPort.writeByte(status);

    // Deactivate auxiliary device
    waitControl();
    controlPort.writeByte(0xa7);

    // Activate keyboard interrupts and enable keyboard
    waitControl();
    controlPort.writeByte(0x20); // Read controller status byte
    waitData();
    status = dataPort.readByte() | 0x01; // Activate keyboard interrupts
    status &= ~0x10; // Rnable keyboard

    waitControl();
    controlPort.writeByte(0x60);
    waitControl();
    dataPort.writeByte(status);
}

int16_t Mouse::read() {
    return inputStream.read();
}

int32_t Mouse::read(uint8_t *targetBuffer, uint32_t offset, uint32_t length) {
    return inputStream.read(targetBuffer, offset, length);
}

}