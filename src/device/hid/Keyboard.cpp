/*
 * Copyright (C) 2018-2022 Heinrich-Heine-Universitaet Duesseldorf,
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
#include "kernel/system/System.h"
#include "filesystem/memory/StreamNode.h"
#include "Keyboard.h"
#include "lib/util/io/Key.h"
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

Kernel::Logger Keyboard::log = Kernel::Logger::get("Keyboard");

Keyboard::Keyboard(Ps2Controller &controller) : Ps2Device(controller, Ps2Controller::FIRST), Util::Stream::FilterInputStream(inputStream) {
    outputStream.connect(inputStream);
}

Keyboard* Keyboard::initialize(Ps2Controller &controller) {
    auto *keyboard = new Keyboard(controller);
    if (!controller.isPortAvailable(Ps2Controller::FIRST)) {
        log.error("First port of PS/2 controller is not available");
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
            log.info("Keyboard has been reset and self test result is OK");
            break;
        } else if (reply == SELF_TEST_FAILED_1 || reply == SELF_TEST_FAILED_2) {
            log.error("Keyboard has been reset but self test result is error code [0x%02x]", reply);
            delete keyboard;
            return nullptr;
        }

        count++;
    } while (count < 10);

    if (!resetAcknowledged || count == 10) {
        log.warn("Failed to reset keyboard -> Keyboard might not be connected");
    }

    // Identify keyboard
    if (!keyboard->writeKeyboardCommand(DISABLE_SCANNING) || !keyboard->writeKeyboardCommand(IDENTIFY)) {
        log.warn("Failed to identify keyboard");
    } else {
        auto type = keyboard->readByte();
        auto subtype = keyboard->readByte();
        if (type == AT_KEYBOARD) {
            log.info("Detected AT keyboard with need for translation");
            controller.enableKeyboardTranslation();
        } else if (type == MF2_KEYBOARD) {
            if (subtype == 0x83) {
                log.info("Detected standard MF2 keyboard");
            } else if (subtype == 0x41 || subtype == 0xc1) {
                log.info("Detected MF2 keyboard with need for translation");
                controller.enableKeyboardTranslation();
            } else {
                log.warn("Detected MF2 keyboard with unknown subtype [0x%02x] -> Assuming translation is not needed",
                         subtype);
            }
        } else {
            log.error("Device connected to first PS/2 port reports as [0x%02x:0x%02x], which is not a valid keyboard", type, subtype);
            delete keyboard;
            return nullptr;
        }
    }

    // Setup keyboard
    if (!keyboard->writeKeyboardCommand(SET_DEFAULT_PARAMETERS)) {
        log.warn("Failed set default parameters");
    }
    if (!keyboard->writeKeyboardCommand(SCAN_CODE_SET, 1)) {
        log.warn("Failed to set scancode set");
    }
    if (!keyboard->writeKeyboardCommand(SET_TYPEMATIC_SPEED, 0)) {
        log.warn("Failed to set repeat rate");
    }
    if (!keyboard->writeKeyboardCommand(SET_LED, 0)) {
        log.warn("Failed to disable LEDs");
    }
    if (!keyboard->writeKeyboardCommand(ENABLE_SCANNING)) {
        log.warn("Failed to enable scanning -> Keyboard might not be working");
    }

    auto *streamNode = new Filesystem::Memory::StreamNode("keyboard", keyboard);
    auto &filesystem = Kernel::System::getService<Kernel::FilesystemService>().getFilesystem();
    auto &driver = filesystem.getVirtualDriver("/device");
    bool success = driver.addNode("/", streamNode);

    if (!success) {
        log.error("Keyboard: Failed to add node");
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
    auto &interruptService = Kernel::System::getService<Kernel::InterruptService>();
    interruptService.assignInterrupt(Kernel::InterruptDispatcher::KEYBOARD, *this);
    interruptService.allowHardwareInterrupt(Pic::Interrupt::KEYBOARD);
}

void Keyboard::trigger(const Kernel::InterruptFrame &frame) {
    uint8_t control = controller.readControlByte();
    if (!(control & 0x01) || (control & 0x20)) {
        return;
    }

    uint8_t data = controller.readDataByte();
    outputStream.write(data);
}

}