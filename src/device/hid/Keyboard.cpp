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
#include "device/hid/Key.h"
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

uint8_t Keyboard::normalTab[] = {
        0, 0, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', 225, 39, '\b',
        '\t', 'q', 'w', 'e', 'r', 't', 'z', 'u', 'i', 'o', 'p', 129, '+', '\n',
        0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', 148, 132, '^', 0, '#',
        'y', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '-', 0,
        '*', 0, ' ', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '-',
        0, 0, 0, '+', 0, 0, 0, 0, 0, 0, 0, '<', 0, 0
};

uint8_t Keyboard::shiftTab[] = {
        0, 0, '!', '"', 21, '$', '%', '&', '/', '(', ')', '=', '?', 96, 0,
        0, 'Q', 'W', 'E', 'R', 'T', 'Z', 'U', 'I', 'O', 'P', 154, '*', 0,
        0, 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', 153, 142, 248, 0, 39,
        'Y', 'X', 'C', 'V', 'B', 'N', 'M', ';', ':', '_', 0,
        0, 0, ' ', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '>', 0, 0
};

uint8_t Keyboard::altTab[] = {
        0, 0, 0, 253, 0, 0, 0, 0, '{', '[', ']', '}', '\\', 0, 0,
        0, '@', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '~', 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 230, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '|', 0, 0
};

uint8_t Keyboard::asciiNumTab[] = {
        '7', '8', '9', '-', '4', '5', '6', '+', '1', '2', '3', '0', ','
};

uint8_t Keyboard::scanNumTab[] = {
        8, 9, 10, 53, 5, 6, 7, 27, 2, 3, 4, 11, 51
};

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

bool Keyboard::decodeKey(uint8_t code) {
    bool done = false;
    bool isBreak =(code & BREAK_BIT);

    if (isBreak) {
        gather.setPressed(false);
    } else {
        gather.setPressed(true);
    }

    if (code == PREFIX1 || code == PREFIX2) {
        prefix = code;
        return false;
    }

    if (code & BREAK_BIT) {
        code &= ~BREAK_BIT;

        switch(code) {
            case 42:
            case 54:
                gather.setShift(false);
                break;
            case 56:
                if(prefix == PREFIX1) {
                    gather.setAltRight(false);
                } else {
                    gather.setAltLeft(false);
                }
                break;
            case 29:
                if(prefix == PREFIX1) {
                    gather.setCtrlRight(false);
                } else {
                    gather.setCtrlLeft(false);
                }
                break;
            default:
                return false;
        }

        prefix = 0;

        return false;
    }

    switch (code) {
        case 42:
        case 54:
            gather.setShift(true);
            break;
        case 56:
            if(prefix == PREFIX1) {
                gather.setAltRight(true);
            } else {
                gather.setAltLeft(true);
            }
            break;
        case 29:
            if(prefix == PREFIX1) {
                gather.setCtrlRight(true);
            } else {
                gather.setCtrlLeft(true);
            }
            break;
        case 58:
            gather.setCapsLock(!gather.getCapsLock());
            setLed(CAPS_LOCK, gather.getCapsLock());
            break;
        case 70:
            gather.setScrollLock(!gather.getScrollLock());
            setLed(SCROLL_LOCK, gather.getScrollLock());
            break;
        case 69:
            if(gather.getCtrlLeft()) {
                getAsciiCode(code);
                done = true;
            }
            else {
                gather.setNumLock(!gather.getNumLock());
                setLed(NUM_LOCK, gather.getNumLock());
            }
            break;

        default:
            getAsciiCode(code);
            done = true;
    }

    prefix = 0;
    return done;
}

void Keyboard::getAsciiCode(uint8_t code) {
    if(code == 53 && prefix == PREFIX1) {
        gather.setAscii('/');
        gather.setScancode(Key::DIV);
    } else if(gather.getNumLock() && !prefix && code >= 71 && code <= 83) {
        gather.setAscii(asciiNumTab[code-71]);
        gather.setScancode(scanNumTab[code-71]);
    } else if(gather.getAltRight()) {
        gather.setAscii(altTab[code]);
        gather.setScancode(code);
    } else if(gather.getShift()) {
        gather.setAscii(shiftTab[code]);
        gather.setScancode(code);
    } else if(gather.getCapsLock()) {
        if((code >= 16 && code <= 26) ||(code >= 30 && code <= 40) ||(code >= 44 && code <= 50)) {
            gather.setAscii(shiftTab[code]);
            gather.setScancode(code);
        } else {
            gather.setAscii(normalTab[code]);
            gather.setScancode(code);
        }
    } else {
        gather.setAscii(normalTab[code]);
        gather.setScancode(code);
    }
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
    if (decodeKey(data)) {
        auto c = gather.getAscii();
        if (c == 0) {
            switch (gather.getScancode()) {
                case 0x48:
                    outputStream.write(reinterpret_cast<const uint8_t*>("\u001b[1A"), 0, 4);
                    break;
                case 0x50:
                    outputStream.write(reinterpret_cast<const uint8_t*>("\u001b[1B"), 0, 4);
                    break;
                case 0x4D:
                    outputStream.write(reinterpret_cast<const uint8_t*>("\u001b[1C"), 0, 4);
                    break;
                case 0x4B:
                    outputStream.write(reinterpret_cast<const uint8_t*>("\u001b[1D"), 0, 4);
                    break;
            }
        } else {
            if (gather.getCtrl()) {
                c &= 0x1f;
            }

            outputStream.write(c);
        }
    }
}

}