/*
 * Copyright (C) 2018-2024 Heinrich-Heine-Universitaet Duesseldorf,
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

#include "kernel/service/InterruptService.h"
#include "filesystem/memory/StreamNode.h"
#include "kernel/service/FilesystemService.h"
#include "device/hid/Ps2Controller.h"
#include "device/hid/Ps2Device.h"
#include "filesystem/Filesystem.h"
#include "filesystem/memory/MemoryDriver.h"
#include "kernel/log/Log.h"
#include "device/interrupt/InterruptRequest.h"
#include "kernel/interrupt/InterruptVector.h"
#include "kernel/service/Service.h"
#include "lib/util/base/String.h"

namespace Kernel {
struct InterruptFrame;
}  // namespace Kernel

namespace Device {

Mouse::Mouse(Ps2Controller &controller) : Ps2Device(controller, Ps2Controller::SECOND), Util::Io::FilterInputStream(inputStream), inputBuffer(BUFFER_SIZE), inputStream(inputBuffer) {}

Mouse* Mouse::initialize(Ps2Controller &controller) {
    auto *mouse = new Mouse(controller);
    if (!controller.isPortAvailable(Ps2Controller::SECOND)) {
        LOG_ERROR("Second port of PS/2 controller is not available");
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
            continue;
        } else if (reply == SELF_TEST_PASSED) {
            LOG_INFO("Mouse has been reset and self test result is OK");
            break;
        } else if (reply == SELF_TEST_FAILED_1 || reply == SELF_TEST_FAILED_2) {
            LOG_ERROR("Mouse has been reset but self test result is error code [0x%02x]", reply);
            delete mouse;
            return nullptr;
        }

        count++;
    } while (count < 5);

    if (!resetAcknowledged || count == 5) {
        LOG_ERROR("Failed to reset mouse -> Assuming no mouse is connected");
        delete mouse;
        return nullptr;
    }

    mouse->readByte(); // The mouse seems to send another byte afterward, which is 0x00 and can be discarded

    // Setup mouse
    if (!mouse->writeMouseCommand(SET_DEFAULT_PARAMETERS)) {
        LOG_WARN("Failed to set default parameters");
    }
    if (!mouse->writeMouseCommand(SET_RESOLUTION, 0x02)) {
        LOG_WARN("Failed to set resolution");
    }
    if (!mouse->writeMouseCommand(SET_SAMPLING_RATE, 80)) {
        LOG_WARN("Failed to set sampling rate");
    }

    if (!mouse->writeMouseCommand(ENABLE_DATA_REPORTING)) {
        LOG_ERROR("Failed to enable data reporting -> Assuming no mouse is connected");
        delete mouse;
        return nullptr;
    }

    controller.flushOutputBuffer();

    mouse->activateScrollWheel();
    if (mouse->type == MOUSE_WITH_SCROLL_WHEEL) {
        mouse->activateAdditionalButtons();
    }

    if (mouse->type == STANDARD_MOUSE) {
        LOG_INFO("Detected standard 3-button mouse without scroll wheel");
    } else if (mouse->type == MOUSE_WITH_SCROLL_WHEEL) {
        LOG_INFO("Detected 3-button mouse with scroll wheel");
    } else if (mouse->type == FIVE_BUTTON_MOUSE) {
        LOG_INFO("Detected 5-button mouse with scroll wheel");
    }

    auto *streamNode = new Filesystem::Memory::StreamNode("mouse", mouse);
    auto &filesystem = Kernel::Service::getService<Kernel::FilesystemService>().getFilesystem();
    auto &driver = filesystem.getVirtualDriver("/device");
    bool success = driver.addNode("/", streamNode);

    if (!success) {
        LOG_ERROR("Mouse: Failed to add node");
        delete streamNode;
        return nullptr;
    }

    return mouse;
}

void Mouse::plugin() {
    auto &interruptService = Kernel::Service::getService<Kernel::InterruptService>();
    interruptService.assignInterrupt(Kernel::InterruptVector::MOUSE, *this);
    interruptService.allowHardwareInterrupt(Device::InterruptRequest::MOUSE);
}
void Mouse::trigger([[maybe_unused]] const Kernel::InterruptFrame &frame, [[maybe_unused]] Kernel::InterruptVector slot) {
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
            dx = data;
            cycle++;
            break;
        case 3:
            dy = data;

            if (type == STANDARD_MOUSE) {
                // Write data: 1. button mask, 2. relative x-movement, 3. relative y-movement, 4. placeholder
                if (BUFFER_SIZE - inputBuffer.size() >= 4) {
                    inputBuffer.offer(flags);
                    inputBuffer.offer(dx);
                    inputBuffer.offer(dy);
                    inputBuffer.offer(0);
                }

                // Reset cycle
                cycle = 1;
            } else {
                cycle++;
            }

            break;
        case 4:
            // Write data: 1. button mask, 2. relative x-movement, 3. relative y-movement, 4. scroll wheel and additional button mask
            if (BUFFER_SIZE - inputBuffer.size() >= 4) {
                inputBuffer.offer(flags);
                inputBuffer.offer(dx);
                inputBuffer.offer(dy);
                inputBuffer.offer(data);
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

void Mouse::activateScrollWheel() {
    // Magic sequence to activate scroll wheel
    if (!writeMouseCommand(SET_SAMPLING_RATE, 200)) {
        LOG_WARN("Failed to set sampling rate");
    }
    if (!writeMouseCommand(SET_SAMPLING_RATE, 100)) {
        LOG_WARN("Failed to set sampling rate");
    }
    if (!writeMouseCommand(SET_SAMPLING_RATE, 80)) {
        LOG_WARN("Failed to set sampling rate");
    }

    controller.flushOutputBuffer();

    // Refresh mouse type
    type = identify();
}

void Mouse::activateAdditionalButtons() {
    // Magic sequence to activate additional buttons
    if (!writeMouseCommand(SET_SAMPLING_RATE, 200)) {
        LOG_WARN("Failed to set sampling rate");
    }
    if (!writeMouseCommand(SET_SAMPLING_RATE, 200)) {
        LOG_WARN("Failed to set sampling rate");
    }
    if (!writeMouseCommand(SET_SAMPLING_RATE, 80)) {
        LOG_WARN("Failed to set sampling rate");
    }

    controller.flushOutputBuffer();

    // Refresh mouse type
    type = identify();
}

Ps2Device::DeviceType Mouse::identify() {
    if (!writeMouseCommand(DISABLE_DATA_REPORTING) || !writeMouseCommand(IDENTIFY)) {
        LOG_WARN("Failed to identify mouse -> Assuming standard 3-button mouse");
        return STANDARD_MOUSE;
    }

    auto mouseType = readByte();
    readByte(); // Discard subtype

    if (!writeMouseCommand(ENABLE_DATA_REPORTING)) {
        LOG_ERROR("Failed to enable data reporting -> Mouse might not be working");
    }

    controller.flushOutputBuffer();
    return static_cast<DeviceType>(mouseType);
}

}