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

#ifndef HHUOS_LIB_UTIL_IO_DELAYOUT_H
#define HHUOS_LIB_UTIL_IO_DELAYOUT_H

#include "util/io/key/KeyboardLayout.h"

namespace Util {
namespace Io {

/// Keyboard layout for German QWERTZ keyboards.
class DeLayout : public KeyboardLayout {

public:
    /// Create a new German keyboard layout instance.
    DeLayout() : KeyboardLayout(NORMAL_TABLE, SHIFT_TABLE, ALT_TABLE, NUMPAD_TABLE) {}

private:

    static uint8_t NORMAL_TABLE[89];
    static uint8_t SHIFT_TABLE[89];
    static uint8_t ALT_TABLE[89];
    static uint8_t NUMPAD_TABLE[13];
};

}
}

#endif
