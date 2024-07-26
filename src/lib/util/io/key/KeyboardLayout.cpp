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

#include "KeyboardLayout.h"

#include "lib/util/io/key/Key.h"
#include "lib/util/io/key/KeyDecoder.h"

void Util::Io::KeyboardLayout::parseAsciiCode(uint8_t scancode, uint8_t prefix, Util::Io::Key &key) const {
    if (scancode == 53 && prefix == KeyDecoder::PREFIX1) {
        key.setAscii('/');
        key.setScancode(Util::Io::Key::DIV);
    } else if (key.getNumLock() && !prefix && scancode >= 71 && scancode <= 83) {
        key.setAscii(asciiNumTab[scancode - 71]);
        key.setScancode(scanNumTab[scancode - 71]);
    } else if (key.getAltRight()) {
        key.setAscii(altTab[scancode]);
        key.setScancode(scancode);
    } else if (key.getShift()) {
        key.setAscii(shiftTab[scancode]);
        key.setScancode(scancode);
    } else if (key.getCapsLock()) {
        if ((scancode >= 16 && scancode <= 26) ||(scancode >= 30 && scancode <= 40) ||(scancode >= 44 && scancode <= 50)) {
            key.setAscii(shiftTab[scancode]);
            key.setScancode(scancode);
        } else {
            key.setAscii(normalTab[scancode]);
            key.setScancode(scancode);
        }
    } else {
        key.setAscii(normalTab[scancode]);
        key.setScancode(scancode);
    }
}
