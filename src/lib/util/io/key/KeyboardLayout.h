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
 *
 * The key decoder is based on code taken from the "Operating Systems" lecture at TU Dortmund.
 * The original code has been implemented by Olaf Spinczyk and its documentation is available here:
 * https://sys-sideshow.cs.tu-dortmund.de/lehre/ws24/bsb/aufgaben/1/classKeyDecoder.html
 */

#ifndef HHUOS_LIB_UTIL_IO_KEYBOARDLAYOUT_H
#define HHUOS_LIB_UTIL_IO_KEYBOARDLAYOUT_H

#include <stdint.h>

#include "util/io/key/Key.h"

namespace Util {
namespace Io {

/// Base class for keyboard layouts, defining how scancodes are mapped to ASCII characters.
/// This class provides the `parseAsciiCode()` method to translate a scancode into a `Key` object,
/// taking into account modifier keys such as Shift, Alt, and Caps Lock.
/// The `KeyDecoder` uses this class as the last step to decode scancodes into `Key` objects,
/// after it has processed prefixes and modifier states.
///
/// Subclasses must only provide the ASCII mapping tables for normal, shifted, Alt, and numpad keys.
/// Everything else is handled by this base class.
class KeyboardLayout {

public:
    /// Translate a scancode and prefix into a `Key` object, considering the current modifier states.
    /// The `Key` object is updated with the corresponding ASCII code and scancode.
    void parseKey(uint8_t scancode, uint8_t prefix, Key &key) const;

protected:

    /// Create a keyboard layout with the given ASCII mapping tables.
    /// The tables are copied into internal buffers of this instance.
    /// Each table must have the following sizes:
    /// - normalTable: 89 entries
    /// - shiftTable: 89 entries
    /// - altTable: 89 entries
    /// - numpadTable: 13 entries
    /// This constructor should be called by subclasses only and is thus protected.
    KeyboardLayout(const uint8_t normalTable[89], const uint8_t shiftTable[89],
        const uint8_t altTable[89], const uint8_t numpadTable[13]);

private:

    uint8_t normalTable[89]{};
    uint8_t shiftTable[89]{};
    uint8_t altTable[89]{};
    uint8_t numpadTable[13]{};

    static constexpr uint8_t NUMPAD_SCANCODE_TABLE[13] = {
        8, 9, 10, 53, 5, 6, 7, 27, 2, 3, 4, 11, 51
    };
};

}
}

#endif
