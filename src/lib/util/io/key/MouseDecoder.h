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

#ifndef HHUOS_MOUSEDECODER_H
#define HHUOS_MOUSEDECODER_H

#include <stdint.h>

namespace Util::Io {

namespace Mouse {

enum Button : uint8_t {
    LEFT_BUTTON = 0x01,
    RIGHT_BUTTON = 0x02,
    MIDDLE_BUTTON = 0x04,
    BUTTON_4 = 0x10,
    BUTTON_5 = 0x20
};

enum ScrollDirection : uint8_t {
    DOWN = 0x01,
    UP = 0x0f,
    LEFT = 0x02,
    RIGHT = 0x0e
};

struct Update {
    uint8_t buttons;
    int16_t xMovement;
    int16_t yMovement;
    ScrollDirection scroll;
};

}

class MouseDecoder {

public:

    /**
     * Default Constructor.
     * Deleted, as this class has only static members.
     */
    MouseDecoder() = delete;

    /**
     * Copy Constructor.
     */
    MouseDecoder(const MouseDecoder &other) = delete;

    /**
     * Assignment operator.
     */
    MouseDecoder &operator=(const MouseDecoder &other) = delete;

    /**
     * Destructor.
     * Deleted, as this class has only static members.
     */
    ~MouseDecoder() = delete;

    static Mouse::Update decode(const uint8_t bytes[4]);

private:

    enum Flag : uint8_t {
        X_SIGN = 0x10,
        Y_SIGN = 0x20,
        X_OVERFLOW = 0x40,
        Y_OVERFLOW = 0x80
    };
};

}

#endif
