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

#ifndef HHUOS_USLAYOUT_H
#define HHUOS_USLAYOUT_H

#include "lib/util/io/key/KeyboardLayout.h"
#include "lib/util/base/Address.h"

namespace Util::Io {

class UsLayout : public KeyboardLayout {

public:
    /**
     * Default Constructor.
     */
    UsLayout() : KeyboardLayout() {
        uint8_t normalTab[89] = {
                0, 0, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
                '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
                0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', 0, '\\',
                'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0,
                '*', 0, ' ', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '-',
                0, 0, 0, '+', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
        };

        uint8_t shiftTab[89] = {
                0, 0, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', 0,
                0, 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', 0,
                0, 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~', 0, '|',
                'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', 0,
                0, 0, ' ', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
        };

        uint8_t altTab[89] = {
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
        };

        uint8_t asciiNumTab[13] = {
                '7', '8', '9', '-', '4', '5', '6', '+', '1', '2', '3', '0', ','
        };

        Util::Address(KeyboardLayout::normalTab).copyRange(Util::Address(normalTab), sizeof(KeyboardLayout::normalTab));
        Util::Address(KeyboardLayout::shiftTab).copyRange(Util::Address(shiftTab), sizeof(KeyboardLayout::shiftTab));
        Util::Address(KeyboardLayout::altTab).copyRange(Util::Address(altTab), sizeof(KeyboardLayout::altTab));
        Util::Address(KeyboardLayout::asciiNumTab).copyRange(Util::Address(asciiNumTab), sizeof(KeyboardLayout::asciiNumTab));
    }

    /**
     * Copy Constructor.
     */
    UsLayout(const UsLayout &other) = delete;

    /**
     * Assignment operator.
     */
    UsLayout &operator=(const UsLayout &other) = delete;

    /**
     * Destructor.
     */
    ~UsLayout() = default;

private:
};


}

#endif
