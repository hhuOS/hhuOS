/*
 * Copyright (C) 2018-2024 Heinrich-Heine-Universitaet Duesseldorf,
 * Institute of Computer Science, Department Operating Systems
 * Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schoettner; Olaf Spinczyk, TU Dortmund
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
#include "kernel/service/FilesystemService.h"
#include "filesystem/memory/StreamNode.h"
#include "Keyboard.h"
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

Keyboard::Keyboard(Ps2Controller &controller) : Ps2Device(controller, Ps2Controller::FIRST), Util::Io::FilterInputStream(inputStream), keyBuffer(BUFFER_SIZE), inputStream(keyBuffer) {}

Keyboard* Keyboard::initialize(Ps2Controller &controller) {
    auto *keyboard = new Keyboard(controller);
    if (!controller.isPortAvailable(Ps2Controller::FIRST)) {
        LOG_ERROR("First port of PS/2 controller is not available");
        delete keyboard;
        return nullptr;
    }

    // Reset keyboard
    uint8_t count = 0;
    bool resetAcknowledged = keyboard->writeKeyboardCommand(RESET);
    do {
        auto reply = keyboard->readByte();
        if (reply == ACK) {
            resetAcknowledged = true;
        } if (reply == SELF_TEST_PASSED) {
            LOG_INFO("Keyboard has been reset and self test result is OK");
            break;
        } else if (reply == SELF_TEST_FAILED_1 || reply == SELF_TEST_FAILED_2) {
            LOG_ERROR("Keyboard has been reset but self test result is error code [0x%02x]", reply);
            delete keyboard;
            return nullptr;
        }

        count++;
    } while (count < 5);

    if (!resetAcknowledged || count == 5) {
        LOG_WARN("Failed to reset keyboard -> Keyboard might not be connected");
    }

    // Identify keyboard
    if (!keyboard->writeKeyboardCommand(DISABLE_SCANNING) || !keyboard->writeKeyboardCommand(IDENTIFY)) {
        LOG_WARN("Failed to identify keyboard");
    } else {
        auto type = keyboard->readByte();
        auto subtype = keyboard->readByte();
        if (type == AT_KEYBOARD) {
            LOG_INFO("Detected AT keyboard with need for translation");
            controller.enableKeyboardTranslation();
        } else if (type == MF2_KEYBOARD) {
            if (subtype == 0x83) {
                LOG_INFO("Detected standard MF2 keyboard");
            } else if (subtype == 0x41 || subtype == 0xc1) {
                LOG_INFO("Detected MF2 keyboard with need for translation");
                controller.enableKeyboardTranslation();
            } else {
                LOG_WARN("Detected MF2 keyboard with unknown subtype [0x%02x] -> Assuming translation is not needed", subtype);
            }
        } else {
            LOG_ERROR("Device connected to first PS/2 port reports as [0x%02x:0x%02x], which is not a valid keyboard", type, subtype);
            delete keyboard;
            return nullptr;
        }
    }

    // Setup keyboard
    if (!keyboard->writeKeyboardCommand(SET_DEFAULT_PARAMETERS)) {
        LOG_WARN("Failed set default parameters");
    }
    if (!keyboard->writeKeyboardCommand(SCAN_CODE_SET, 1)) {
        LOG_WARN("Failed to set scancode set");
    }
    if (!keyboard->writeKeyboardCommand(SET_TYPEMATIC_SPEED, 0)) {
        LOG_WARN("Failed to set repeat rate");
    }
    if (!keyboard->writeKeyboardCommand(SET_LED, 0)) {
        LOG_WARN("Failed to disable LEDs");
    }
    if (!keyboard->writeKeyboardCommand(ENABLE_SCANNING)) {
        LOG_WARN("Failed to enable scanning -> Keyboard might not be working");
    }

    auto *streamNode = new Filesystem::Memory::StreamNode("keyboard", keyboard);
    auto &filesystem = Kernel::Service::getService<Kernel::FilesystemService>().getFilesystem();
    auto &driver = filesystem.getVirtualDriver("/device");
    bool success = driver.addNode("/", streamNode);

    if (!success) {
        LOG_ERROR("Keyboard: Failed to add node");
        delete streamNode;
        return nullptr;
    }

    return keyboard;
}

bool Keyboard::writeKeyboardCommand(Device::Keyboard::Command command) {
    uint8_t count = 0;
    Reply reply;

    do {
        reply = static_cast<Reply>(writeCommand(command));
        count++;
    } while (reply == RESEND_LAST_COMMAND && count < 3);

    return reply == ACK;
}

bool Keyboard::writeKeyboardCommand(Command command, uint8_t data) {
    uint8_t count = 0;
    Reply reply;

    do {
        reply = static_cast<Reply>(writeCommand(command, data));
        count++;
    } while (reply == RESEND_LAST_COMMAND && count < 3);

    return reply == ACK;
}

void Keyboard::setLed(Led led, bool on) {
    auto tmpLeds = on ? leds | led : leds & ~led;
    if (writeKeyboardCommand(SET_LED, tmpLeds)) {
        leds = tmpLeds;
    }
}

void Keyboard::plugin() {
    auto &interruptService = Kernel::Service::getService<Kernel::InterruptService>();
    interruptService.assignInterrupt(Kernel::InterruptVector::KEYBOARD, *this);
    interruptService.allowHardwareInterrupt(Device::InterruptRequest::KEYBOARD);
}

void Keyboard::trigger([[maybe_unused]] const Kernel::InterruptFrame &frame, [[maybe_unused]] Kernel::InterruptVector slot) {
    auto control = controller.readControlByte();
    if (!(control & 0x01) || (control & 0x20)) {
        return;
    }

    auto data = controller.readDataByte();
    keyBuffer.offer(data);
}

}