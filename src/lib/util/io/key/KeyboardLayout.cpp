/*
 * Copyright (C) 2017-2025 Heinrich Heine University Düsseldorf,
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

#include "KeyboardLayout.h"

#include "base/Address.h"
#include "io/key/Key.h"
#include "io/key/KeyDecoder.h"

void Util::Io::KeyboardLayout::parseKey(const uint8_t scancode, const uint8_t prefix, Key &key) const {
    if (scancode == 53 && prefix == KeyDecoder::PREFIX1) {
        key.setAscii('/');
        key.setScancode(53);
    } else if (key.getNumLock() && !prefix && scancode >= 71 && scancode <= 83) {
        key.setAscii(numpadTable[scancode - 71]);
        key.setScancode(NUMPAD_SCANCODE_TABLE[scancode - 71]);
    } else if (key.getAltRight()) {
        key.setAscii(altTable[scancode]);
        key.setScancode(scancode);
    } else if (key.getShift()) {
        key.setAscii(shiftTable[scancode]);
        key.setScancode(scancode);
    } else if (key.getCapsLock()) {
        if ((scancode >= 16 && scancode <= 26) ||
            (scancode >= 30 && scancode <= 40) ||
            (scancode >= 44 && scancode <= 50))
        {
            key.setAscii(shiftTable[scancode]);
            key.setScancode(scancode);
        } else {
            key.setAscii(normalTable[scancode]);
            key.setScancode(scancode);
        }
    } else {
        key.setAscii(normalTable[scancode]);
        key.setScancode(scancode);
    }
}

Util::Io::KeyboardLayout::KeyboardLayout(const uint8_t normalTable[89], const uint8_t shiftTable[89],
    const uint8_t altTable[89], const uint8_t numpadTable[13])
{
    Address(KeyboardLayout::normalTable).copyRange(Address(normalTable), sizeof(KeyboardLayout::normalTable));
    Address(KeyboardLayout::shiftTable).copyRange(Address(shiftTable), sizeof(KeyboardLayout::shiftTable));
    Address(KeyboardLayout::altTable).copyRange(Address(altTable), sizeof(KeyboardLayout::altTable));
    Address(KeyboardLayout::numpadTable).copyRange(Address(numpadTable), sizeof(KeyboardLayout::numpadTable));
}
