/*
 * Copyright (C) 2018-2022 Heinrich-Heine-Universitaet Duesseldorf,
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

#ifndef HHUOS_KEYDECODER_H
#define HHUOS_KEYDECODER_H

#include <cstdint>
#include "Key.h"

namespace Util::Io {

class KeyDecoder {

public:
    /**
     * Default Constructor.
     */
    KeyDecoder() = default;

    /**
     * Copy Constructor.
     */
    KeyDecoder(const KeyDecoder &other) = delete;

    /**
     * Assignment operator.
     */
    KeyDecoder &operator=(const KeyDecoder &other) = delete;

    /**
     * Destructor.
     */
    ~KeyDecoder() = default;

    bool parseScancode(uint8_t code);

    [[nodiscard]] Key getCurrentKey() const;

private:

    void parseAsciiCode(uint8_t code);

    enum Code : uint8_t {
        BREAK_BIT = 0x80,
        PREFIX1 = 0xe0,
        PREFIX2 = 0xe1
    };

    uint8_t currentPrefix;
    Key currentKey;

    static uint8_t normalTab[];
    static uint8_t shiftTab[];
    static uint8_t altTab[];
    static uint8_t asciiNumTab[];
    static uint8_t scanNumTab[];
};

}

#endif
