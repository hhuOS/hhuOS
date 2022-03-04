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

#include "kernel/interrupt/InterruptDispatcher.h"
#include "device/interrupt/Pic.h"
#include "filesystem/memory/StreamNode.h"
#include "kernel/service/FilesystemService.h"
#include "kernel/system/System.h"
#include "Keyboard.h"

namespace Device {

Kernel::Logger Keyboard::log = Kernel::Logger::get("Keyboard");

uint8_t Keyboard::normalTab[] = {
        0, 0, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', 225, 39, '\b',
        0, 'q', 'w', 'e', 'r', 't', 'z', 'u', 'i', 'o', 'p', 129, '+', '\n',
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

Keyboard::Keyboard(Util::Stream::PipedInputStream &inputStream) {
    setLed(CAPS_LOCK, false);
    setLed(SCROLL_LOCK, false);
    setLed(NUM_LOCK, false);
    setRepeatRate(0, 0);

    outputStream.connect(inputStream);
}

void Keyboard::initialize() {
    log.info("Initializing keyboard");
    auto inputStream = new Util::Stream::PipedInputStream();
    auto streamNode = new Filesystem::Memory::StreamNode("keyboard", inputStream);
    auto keyboard = new Device::Keyboard(*inputStream);

    auto &filesystem = Kernel::System::getService<Kernel::FilesystemService>().getFilesystem();
    auto &driver = filesystem.getVirtualDriver("/device");
    bool success = driver.addNode("/", streamNode);

    if (success) {
        keyboard->plugin();
    } else {
        log.error("Failed to initialize virtual node for keyboard");
        delete streamNode;
        delete keyboard;
    }
}

bool Keyboard::decodeKey(uint8_t code) {
    bool done = false;
    bool isBreak =(code & BREAK_BIT);
    uint32_t scancode = code & ~BREAK_BIT;

    if (isBreak) {
        removeFromBuffer(scancode);
        gather.setPressed(false);
    } else {
        addToBuffer(scancode);
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

void Keyboard::setRepeatRate(uint32_t speed, uint32_t delay) {
    uint32_t status, reply;

    dataPort.writeByte(SET_SPEED);
    do {
        status = controlPort.readByte();
    } while((status & OUTB) == 0);

    reply = dataPort.readByte();
    if(reply == ACK) {
        dataPort.writeByte(static_cast<uint8_t>(((delay & 3) << 5) | (speed & 31)));

        do {
            status = controlPort.readByte();
        } while((status & OUTB) == 0);
    }
}

void Keyboard::setLed(uint8_t led, bool on) {
    uint32_t status, reply;

    dataPort.writeByte(SET_LED);
    do {
        status = controlPort.readByte();
    } while((status & OUTB) == 0);

    reply = dataPort.readByte();

    if(reply == ACK) {
        if(on) {
            leds |= led;
        } else {
            leds &= ~led;
        }

        dataPort.writeByte(leds);
        do {
            status = controlPort.readByte();
        } while((status & OUTB) == 0);
    }
}

uint32_t Keyboard::getKeysPressed() const {
    return keysPressed;
}

bool Keyboard::isKeyPressed(uint32_t scancode) const {
    for (const uint32_t &i : buffer) {
        if (i == scancode) {
            return true;
        }
    }

    return false;
}

void Keyboard::addToBuffer(uint32_t scancode) {
    if(isKeyPressed(scancode)) {
        return;
    }

    for (uint32_t &i : buffer) {
        if (i == 0) {
            i = scancode;
            keysPressed++;
            break;
        }
    }
}

void Keyboard::removeFromBuffer(uint32_t scancode) {
    for (uint32_t &i : buffer) {
        if (i == scancode) {
            i = 0;
            keysPressed--;
            break;
        }
    }
}

void Keyboard::plugin() {
    Util::Memory::Address<uint32_t> address(buffer, BUFFER_SIZE);
    address.setRange(0, BUFFER_SIZE);

    Kernel::InterruptDispatcher::getInstance().assign(Kernel::InterruptDispatcher::KEYBOARD, *this);
    Pic::getInstance().allow(Pic::Interrupt::KEYBOARD);
}

void Keyboard::trigger(Kernel::InterruptFrame &frame) {
    uint8_t control = controlPort.readByte();
    if ((control & 0x1) != 0x1) {
        return;
    }

    if ((control & AUXB)) {
        return;
    }

    uint8_t data = dataPort.readByte();
    if (decodeKey(data)) {
        outputStream.write(gather.getAscii());
    }
}

}