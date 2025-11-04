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

#ifndef HHUOS_LIB_UTIL_IO_KEYDECODER_H
#define HHUOS_LIB_UTIL_IO_KEYDECODER_H

#include <stdint.h>

#include "util/io/key/Key.h"
#include "util/io/key/KeyboardLayout.h"

namespace Util::Io {

/// Decodes keyboard scancodes into `Key` objects, using a specified `KeyboardLayout`.
/// The layout defines how scancodes are mapped to ASCII characters, taking into account
/// modifier keys such as Shift, Alt, and Caps Lock.
/// This class keeps an instance of the current key state, which is updated as scancodes are parsed.
/// With each new byte of a scancode, the `parseScancode()` method must be called to update the state.
/// Once a key is fully parsed, it can be retrieved using `getCurrentKey()`.
class KeyDecoder {

public:
    /// Special scancode values.
    enum Code : uint8_t {
        /// The break bit indicates a key release event.
        BREAK_BIT = 0x80,
        /// Prefix for extended scancodes (e.g., right Alt, right Ctrl).
        PREFIX1 = 0xe0,
        /// Prefix for Pause/Break key.
        PREFIX2 = 0xe1
    };

    /// Create a key decoder instance with the given keyboard layout.
    /// The layout defines how scancodes are translated into ASCII characters.
    explicit KeyDecoder(const KeyboardLayout &layout);

    /// Parse a single scancode byte and update the current key state.
    /// If the key is fully parsed, true is returned. The key can then be retrieved using `getCurrentKey()`.
    /// If more bytes are needed (e.g., for multibyte scancodes), false is returned.
    bool parseScancode(uint8_t code);

    /// Get the currently parsed key.
    /// This method should be called only after `parseScancode()` has returned true.
    /// If the key is not fully parsed yet, a panic is fired.
    [[nodiscard]] Key getCurrentKey() const;

private:

    uint8_t currentPrefix = 0;
    Key currentKey;

    KeyboardLayout layout;
};

}

#endif
