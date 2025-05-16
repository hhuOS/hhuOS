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

#ifndef HHUOS_KEYBOARDLAYOUT_H
#define HHUOS_KEYBOARDLAYOUT_H

#include <stdint.h>

namespace Util {
namespace Io {
class Key;
}  // namespace Io
}  // namespace Util

namespace Util::Io {

class KeyboardLayout {

public:
    /**
     * Default Constructor.
     */
    KeyboardLayout() = default;

    /**
     * Copy Constructor.
     */
    KeyboardLayout(const KeyboardLayout &other) = delete;

    /**
     * Assignment operator.
     */
    KeyboardLayout &operator=(const KeyboardLayout &other) = delete;

    /**
     * Destructor.
     */
    ~KeyboardLayout() = default;

    void parseAsciiCode(uint8_t scancode, uint8_t prefix, Key &key) const;

protected:

    uint8_t normalTab[89];
    uint8_t shiftTab[89]{};
    uint8_t altTab[89]{};
    uint8_t asciiNumTab[13]{};

private:

    uint8_t scanNumTab[13] = {
            8, 9, 10, 53, 5, 6, 7, 27, 2, 3, 4, 11, 51
    };
};

}

#endif
