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

#include "KeyDecoder.h"

#include "lib/util/io/key/Key.h"
#include "lib/util/io/key/KeyboardLayout.h"

namespace Util {
namespace Io {

bool KeyDecoder::parseScancode(uint8_t code) {
    bool done = false;
    const bool isBreak = code & BREAK_BIT;

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
                layout.parseKey(code, currentPrefix, currentKey);
                done = true;
            } else {
                currentKey.setNumLock(!currentKey.getNumLock());
            }
            break;

        default:
            layout.parseKey(code, currentPrefix, currentKey);
            done = true;
    }

    currentPrefix = 0;
    return done;
}

}
}