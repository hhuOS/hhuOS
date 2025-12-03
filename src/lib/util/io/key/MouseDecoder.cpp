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

#include "MouseDecoder.h"

namespace Util {
namespace Io {
namespace MouseDecoder {

enum Flag : uint8_t {
    X_SIGN = 0x10,
    Y_SIGN = 0x20,
    X_OVERFLOW = 0x40,
    Y_OVERFLOW = 0x80
};

Update decode(const uint8_t bytes[4]) {
    Update update{};
    update.buttons = (bytes[0] & 0x07) | (bytes[3] & 0x30);

    if (!(bytes[0] & X_OVERFLOW)) {
        update.xMovement = static_cast<int16_t>(bytes[1] - ((bytes[0] & X_SIGN) << 4));
    }

    if (!(bytes[0] & Y_OVERFLOW)) {
        update.yMovement = static_cast<int16_t>(bytes[2] - ((bytes[0] & Y_SIGN) << 3));
    }

    // TODO: Check 0x40 for horizontal scrolling is not documented, but seems to work in QEMU.
    //       This needs to be validated on real hardware
    switch (bytes[3] & 0x0f) {
        case UP:
            update.scroll = (bytes[3] & 0x40) ? RIGHT : UP;
            break;
        case DOWN:
            update.scroll = (bytes[3] & 0x40) ? LEFT : DOWN;
            break;
        case RIGHT:
            update.scroll = RIGHT;
            break;
        case LEFT:
            update.scroll = LEFT;
            break;
        default:
            break;
    }

    return update;
}

}
}
}