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

#include "KeyboardLayout.h"

namespace Util {
namespace Io {

bool KeyDecoder::parseScancode(uint8_t code) {
    // Keys that are new in the MF II keyboard (compared to the old AT keyboard) send a prefix byte first.
    if (code == PREFIX1 || code == PREFIX2) {
        // If the code is a prefix, store it and wait for the next byte.
        currentPrefix = code;
        return false;
    }

    // Store the current prefix and clear it in the keyboard state.
    // The prefix is only valid for the next key, so we clear it after processing.
    const auto prefix = currentPrefix;
    currentPrefix = 0;

    // The break bit indicates whether a key is pressed or released.
    // If it is set, the key is released, otherwise it is pressed.
    if (code & BREAK_BIT) {
        // Break bit is set -> Key released
        // Remove the break bit to get the actual scancode.
        code &= ~BREAK_BIT;

        // Check modifier keys (Shift, Alt, Control).
        // If a modifier key is released, remove it from the modifier flags.
        // Additionally, we generate a key release event for the modifier key itself.
        switch (code) {
            case 42:
            case 54:
                currentModifiers &= ~KeyEvent::SHIFT;
                decodedKeys.offer(KeyEvent(0, 0, KeyEvent::SHIFT, false));
                return true;
            case 56:
                if (currentPrefix == PREFIX1) {
                    currentModifiers &= ~KeyEvent::ALT_LEFT;
                    decodedKeys.offer(KeyEvent(0, 0, KeyEvent::ALT_LEFT, false));
                } else {
                    currentModifiers &= ~KeyEvent::ALT_RIGHT;
                    decodedKeys.offer(KeyEvent(0, 0, KeyEvent::ALT_RIGHT, false));
                }
                return true;
            case 29: {
                if (currentPrefix == PREFIX1) {
                    currentModifiers &= ~KeyEvent::CTRL_RIGHT;
                    decodedKeys.offer(KeyEvent(0, 0, KeyEvent::CTRL_RIGHT, false));
                } else {
                    currentModifiers &= ~KeyEvent::CTRL_LEFT;
                    decodedKeys.offer(KeyEvent(0, 0, KeyEvent::CTRL_LEFT, false));
                }
                return true;
            }
            case 69:
                // On old keyboards, the Break function could only be reached via Ctrl+NumLock.
                // Modern MF-II keyboards send this code combination when Break is meant.
                // The Break key normally does not deliver an ASCII code, but looking it up does not hurt.
                // In any case, the key is now complete.
                if (currentModifiers & KeyEvent::CTRL_LEFT) { // Break key
                    decodedKeys.offer(layout.parseKey(prefix, code, currentModifiers, false));
                    return true;
                }

                // Ignore key releases of toggle keys (NumLock).
                break;
            default:
                // For any other key, parse the ascii code and set the key as released.
                // We now have parsed a complete key (release), so we return true.
                decodedKeys.offer(layout.parseKey(prefix, code, currentModifiers, false));
                return true;
        }
    } else {
        // Break bit is not set -> Key pressed
        // Check modifier keys (Shift, Alt, Control).
        // If a modifier key is pressed, insert it into the modifier flags.
        // Additionally, we generate a key press event for the modifier key itself.
        switch (code) {
            case 42:
            case 54:
                currentModifiers |= KeyEvent::SHIFT;
                decodedKeys.offer(KeyEvent(0, 0, KeyEvent::SHIFT, true));
                return true;
            case 56:
                if (currentPrefix == PREFIX1) {
                    currentModifiers |= KeyEvent::ALT_LEFT;
                    decodedKeys.offer(KeyEvent(0, 0, KeyEvent::ALT_LEFT, true));
                } else {
                    currentModifiers |= KeyEvent::ALT_RIGHT;
                    decodedKeys.offer(KeyEvent(0, 0, KeyEvent::ALT_RIGHT, true));
                }
                return true;
            case 29: {
                if (currentPrefix == PREFIX1) {
                    currentModifiers |= KeyEvent::CTRL_RIGHT;
                    decodedKeys.offer(KeyEvent(0, 0, KeyEvent::CTRL_RIGHT, true));
                } else {
                    currentModifiers |= KeyEvent::CTRL_LEFT;
                    decodedKeys.offer(KeyEvent(0, 0, KeyEvent::CTRL_LEFT, true));
                }
                return true;
            }
            case 58:
                currentModifiers ^= KeyEvent::CAPS_LOCK;
                break;
            case 70:
                currentModifiers ^= KeyEvent::NUM_LOCK;
                break;
            case 69:
                // On old keyboards, the Break function could only be reached via Ctrl+NumLock.
                // Modern MF-II keyboards send this code combination when Break is meant.
                // The Break key normally does not deliver an ASCII code, but looking it up does not hurt.
                // In any case, the key is now complete.
                if (currentModifiers & KeyEvent::CTRL_LEFT) { // Break key
                    decodedKeys.offer(layout.parseKey(prefix, code, currentModifiers, true));
                    return true;
                }

                // NumLock
                currentModifiers ^= KeyEvent::NUM_LOCK;
                break;
            default:
                // For any other key, parse the ascii code and set the key as pressed.
                decodedKeys.offer(layout.parseKey(prefix, code, currentModifiers, true));
                return true;
        }
    }

    return false;
}

}
}