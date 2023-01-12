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

#include "KeyDecoder.h"

namespace Util::Io {

uint8_t KeyDecoder::normalTab[] = {
        0, 0, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', 225, 39, '\b',
        '\t', 'q', 'w', 'e', 'r', 't', 'z', 'u', 'i', 'o', 'p', 129, '+', '\n',
        0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', 148, 132, '^', 0, '#',
        'y', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '-', 0,
        '*', 0, ' ', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '-',
        0, 0, 0, '+', 0, 0, 0, 0, 0, 0, 0, '<', 0, 0
};

uint8_t KeyDecoder::shiftTab[] = {
        0, 0, '!', '"', 21, '$', '%', '&', '/', '(', ')', '=', '?', 96, 0,
        0, 'Q', 'W', 'E', 'R', 'T', 'Z', 'U', 'I', 'O', 'P', 154, '*', 0,
        0, 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', 153, 142, 248, 0, 39,
        'Y', 'X', 'C', 'V', 'B', 'N', 'M', ';', ':', '_', 0,
        0, 0, ' ', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '>', 0, 0
};

uint8_t KeyDecoder::altTab[] = {
        0, 0, 0, 253, 0, 0, 0, 0, '{', '[', ']', '}', '\\', 0, 0,
        0, '@', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '~', 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 230, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '|', 0, 0
};

uint8_t KeyDecoder::asciiNumTab[] = {
        '7', '8', '9', '-', '4', '5', '6', '+', '1', '2', '3', '0', ','
};

uint8_t KeyDecoder::scanNumTab[] = {
        8, 9, 10, 53, 5, 6, 7, 27, 2, 3, 4, 11, 51
};

bool Util::Io::KeyDecoder::parseScancode(uint8_t code) {
    bool done = false;
    bool isBreak = (code & BREAK_BIT);

    if (isBreak) {
        currentKey.setPressed(false);
    } else {
        currentKey.setPressed(true);
    }

    if (code == PREFIX1 || code == PREFIX2) {
        currentPrefix = code;
        return false;
    }

    if (code & BREAK_BIT) {
        code &= ~BREAK_BIT;

        switch (code) {
            case 42:
            case 54:
                currentKey.setShift(false);
                currentPrefix = 0;
                return false;
            case 56:
                if (currentPrefix == PREFIX1) {
                    currentKey.setAltRight(false);
                } else {
                    currentKey.setAltLeft(false);
                }
                currentPrefix = 0;
                return false;
            case 29:
                if (currentPrefix == PREFIX1) {
                    currentKey.setCtrlRight(false);
                } else {
                    currentKey.setCtrlLeft(false);
                }
                currentPrefix = 0;
                return false;
            default:
                break;
        }
    }

    switch (code) {
        case 42:
        case 54:
            currentKey.setShift(true);
            break;
        case 56:
            if (currentPrefix == PREFIX1) {
                currentKey.setAltRight(true);
            } else {
                currentKey.setAltLeft(true);
            }
            break;
        case 29:
            if (currentPrefix == PREFIX1) {
                currentKey.setCtrlRight(true);
            } else {
                currentKey.setCtrlLeft(true);
            }
            break;
        case 58:
            currentKey.setCapsLock(!currentKey.getCapsLock());
            break;
        case 70:
            currentKey.setScrollLock(!currentKey.getScrollLock());
            break;
        case 69:
            if (currentKey.getCtrlLeft()) {
                parseAsciiCode(code);
                done = true;
            } else {
                currentKey.setNumLock(!currentKey.getNumLock());
            }
            break;

        default:
            parseAsciiCode(code);
            done = true;
    }

    currentPrefix = 0;
    return done;
}

void Util::Io::KeyDecoder::parseAsciiCode(uint8_t code) {
    if(code == 53 && currentPrefix == PREFIX1) {
        currentKey.setAscii('/');
        currentKey.setScancode(Util::Io::Key::DIV);
    } else if(currentKey.getNumLock() && !currentPrefix && code >= 71 && code <= 83) {
        currentKey.setAscii(asciiNumTab[code-71]);
        currentKey.setScancode(scanNumTab[code-71]);
    } else if(currentKey.getAltRight()) {
        currentKey.setAscii(altTab[code]);
        currentKey.setScancode(code);
    } else if(currentKey.getShift()) {
        currentKey.setAscii(shiftTab[code]);
        currentKey.setScancode(code);
    } else if(currentKey.getCapsLock()) {
        if((code >= 16 && code <= 26) ||(code >= 30 && code <= 40) ||(code >= 44 && code <= 50)) {
            currentKey.setAscii(shiftTab[code]);
            currentKey.setScancode(code);
        } else {
            currentKey.setAscii(normalTab[code]);
            currentKey.setScancode(code);
        }
    } else {
        currentKey.setAscii(normalTab[code]);
        currentKey.setScancode(code);
    }
}

Util::Io::Key Util::Io::KeyDecoder::getCurrentKey() const {
    return currentKey;
}

}