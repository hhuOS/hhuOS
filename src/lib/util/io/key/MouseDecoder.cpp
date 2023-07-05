/*
 * Copyright (C) 2018-2023 Heinrich-Heine-Universitaet Duesseldorf,
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

#include "MouseDecoder.h"

namespace Util::Io {

Mouse::Update MouseDecoder::decode(const uint8_t bytes[4]) {
    Mouse::Update update{};
    update.buttons = (bytes[0] & 0x07) | (bytes[3] & 0x30);

    if (!(bytes[0] & X_OVERFLOW)) {
        update.xMovement = static_cast<int16_t>(bytes[1] - ((bytes[0] & X_SIGN) << 4));
    }

    if (!(bytes[0] & Y_OVERFLOW)) {
        update.yMovement = static_cast<int16_t>(bytes[2] - ((bytes[0] & Y_SIGN) << 3));
    }

    // TODO: Checking 0x40 for horizontal scrolling is not documented, but seems to work in QEMU.
    //  This needs to be validated on real hardware
    switch (bytes[3] & 0x0f) {
        case Mouse::UP:
            update.scroll = (bytes[3] & 0x40) ? Mouse::RIGHT : Mouse::UP;
            break;
        case Mouse::DOWN:
            update.scroll = (bytes[3] & 0x40) ? Mouse::LEFT : Mouse::DOWN;
            break;
        case Mouse::RIGHT:
            update.scroll = Mouse::RIGHT;
            break;
        case Mouse::LEFT:
            update.scroll = Mouse::LEFT;
            break;
    }

    return update;
}

}