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

#ifndef HHUOS_LIB_UTIL_IO_MOUSEDECODER_H
#define HHUOS_LIB_UTIL_IO_MOUSEDECODER_H

#include <stdint.h>

namespace Util {
namespace Io {

/// Provides functionality to decode mouse packets as per the standard PS/2 mouse protocol.
namespace MouseDecoder {

/// Mouse button values using bit flags.
enum Button : uint8_t {
    /// Bit flag for the left mouse button.
    LEFT_BUTTON = 0x01,
    /// Bit flag for the right mouse button.
    RIGHT_BUTTON = 0x02,
    /// Bit flag for the middle mouse button (usually the scroll wheel button).
    MIDDLE_BUTTON = 0x04,
    /// Bit flag for the fourth mouse button (if present).
    BUTTON_4 = 0x10,
    /// Bit flag for the fifth mouse button (if present).
    BUTTON_5 = 0x20
};

/// Values for vertical and horizontal scroll directions.
enum ScrollDirection : uint8_t {
    /// No scrolling action.
    NONE = 0x00,
    /// Scroll down action.
    DOWN = 0x01,
    /// Scroll up action.
    UP = 0x0f,
    /// Scroll left action.
    LEFT = 0x02,
    /// Scroll right action.
    RIGHT = 0x0e
};

/// A decoded mouse update containing button states, movement deltas, and scroll direction.
struct Update {
    /// Bitwise combination of values from the `Button` enum, indicating which buttons are pressed.
    uint8_t buttons;
    /// Movement in the X direction since the last update (ranges from -255 to 255).
    /// Positive values indicate movement to the right, negative values indicate movement to the left.
    int16_t xMovement;
    /// Movement in the Y direction since the last update (ranges from -255 to 255).
    /// Positive values indicate movement upwards, negative values indicate movement downwards.
    int16_t yMovement;
    /// Indicates the scroll direction using values from the `ScrollDirection` enum.
    /// `NONE` means no scrolling, `UP` and `DOWN` indicate vertical scrolling,
    /// while `LEFT` and `RIGHT` indicate horizontal scrolling.
    ScrollDirection scroll;
};

/// Decode a PS/2 mouse packet into an `Update` structure.
/// An application using the mouse should read 4 bytes from the mouse device
/// and pass them to this function to interpret the mouse actions.
Update decode(const uint8_t bytes[4]);

}

}
}

#endif
