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
#include "device/debug/FirmwareConfiguration.h"
#include "device/hid/Ps2Controller.h"
#include "device/hid/Ps2Device.h"
#include "device/interrupt/Pic.h"
#include "filesystem/core/Filesystem.h"
#include "filesystem/memory/MemoryDriver.h"
#include "kernel/interrupt/InterruptDispatcher.h"
#include "kernel/log/Logger.h"

namespace Kernel {
struct InterruptFrame;
}  // namespace Kernel

namespace Device {

Kernel::Logger Mouse::log = Kernel::Logger::get("Mouse");

Mouse::Mouse(Ps2Controller &controller) : Ps2Device(controller, Ps2Controller::SECOND), Util::Stream::FilterInputStream(inputStream), qemuMode(FirmwareConfiguration::isAvailable()) {
    outputStream.connect(inputStream);
}

Mouse* Mouse::initialize(Ps2Controller &controller) {
    auto *mouse = new Mouse(controller);
    if (!controller.isPortAvailable(Ps2Controller::SECOND)) {
        log.error("Second port of PS/2 controller is not available");
        delete mouse;
        return nullptr;
    }

    // Reset mouse
    uint8_t count = 0;
    bool resetAcknowledged = mouse->writeMouseCommand(RESET);
    do {
        auto reply = mouse->readByte();
        if (reply == ACK) {
            resetAcknowledged = true;
        } else if (reply == SELF_TEST_PASSED) {
            log.info("Mouse has been reset and self test result is OK");
            break;
        } else if (reply == SELF_TEST_FAILED_1 || reply == SELF_TEST_FAILED_2) {
            log.error("Mouse has been reset but self test result is error code [0x%02x]", reply);
            delete mouse;
            return nullptr;
        }

        count++;
    } while (count < 10);

    if (!resetAcknowledged || count == 10) {
        log.warn("Failed to reset mouse -> Mouse might not be connected");
    }

    mouse->readByte(); // The mouse seems to send another byte afterwards, which is 0x00 and can be discarded

    // Identify mouse
    if (!mouse->writeMouseCommand(DISABLE_DATA_REPORTING) || !mouse->writeMouseCommand(IDENTIFY)) {
        log.warn("Failed to identify mouse -> Assuming standard 3-button mouse");
    } else {
        auto type = mouse->readByte();
        auto subtype = mouse->readByte();
        if (type == STANDARD_MOUSE) {
            log.info("Detected standard 3-button mouse");
        } else if (type == MOUSE_WITH_SCROLL_WHEEL) {
            log.info("Detected mouse with scroll wheel");
        } else if (type == FIVE_BUTTON_MOUSE) {
            log.info("Detected 5-button mouse");
        } else {
            log.error("Device connected to second PS/2 port reports as [0x%02x:0x%02x], which is not a valid mouse", type, subtype);
            delete mouse;
            return nullptr;
        }
    }

    // Setup mouse
    if (!mouse->writeMouseCommand(SET_DEFAULT_PARAMETERS)) {
        log.warn("Failed to set default parameters");
    }
    if (!mouse->writeMouseCommand(SET_RESOLUTION, 0x02)) {
        log.warn("Failed to set resolution");
    }
    if (!mouse->writeMouseCommand(SET_SAMPLING_RATE, 80)) {
        log.warn("Failed to set sampling rate");
    }
    if (!mouse->writeMouseCommand(ENABLE_DATA_REPORTING)) {
        log.warn("Failed to enable data reporting -> Mouse might not be working");
    }

    controller.flushOutputBuffer();

    auto *streamNode = new Filesystem::Memory::StreamNode("mouse", mouse);
    auto &filesystem = Kernel::System::getService<Kernel::FilesystemService>().getFilesystem();
    auto &driver = filesystem.getVirtualDriver("/device");
    bool success = driver.addNode("/", streamNode);

    if (!success) {
        log.error("Mouse: Failed to add node");
        delete streamNode;
        return nullptr;
    }

    return mouse;
}

void Mouse::plugin() {
    auto &interruptService = Kernel::System::getService<Kernel::InterruptService>();
    interruptService.assignInterrupt(Kernel::InterruptDispatcher::MOUSE, *this);
    interruptService.allowHardwareInterrupt(Pic::Interrupt::MOUSE);
}

void Mouse::trigger(const Kernel::InterruptFrame &frame) {
    auto status = controller.readControlByte();
    if (!(status & 0x20)) {
        return;
    }
    
    auto data = controller.readDataByte();
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
            // Check if signed
            if (flags & 0x20) {
                // Extend unsigned 8-bit data value to unsigned 16-bit in twos complement
                dx = static_cast<int16_t>(data | 0xff00);
            } else {
                dx = data;
            }

            cycle++;
            break;
        case 3:
            // Check if signed
            if(flags & 0x10) {
                // Extend unsigned 8-bit data value to unsigned 16-bit in twos complement
                dy = static_cast<int16_t>(data | 0xff00);
            } else {
                dy = data;
            }

            // If there was an x or y overflow, discard this 'event'
            if (flags & 0x40 || flags & 0x80) {
                cycle = 1;
                return;
            }

            // Write data: 1. button mask, 2. relative x-movement, 3. relative y-movement (inverted)
            outputStream.write(flags & 0x07);

            if (qemuMode) {
                outputStream.write(dy);
                outputStream.write(-dx);
            } else {
                outputStream.write(dx);
                outputStream.write(-dy);
            }

            // Reset cycle
            cycle = 1;
            break;
    }
}

bool Mouse::writeMouseCommand(Mouse::Command command) {
    auto reply = writeCommand(command);
    return reply == ACK;
}

bool Mouse::writeMouseCommand(Mouse::Command command, uint8_t data) {
    auto reply = writeCommand(command, data);
    return reply == ACK;
}

}