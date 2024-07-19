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

#ifndef HHUOS_DELAYOUT_H
#define HHUOS_DELAYOUT_H

#include "lib/util/io/key/KeyboardLayout.h"
#include "lib/util/base/Address.h"

namespace Util::Io {

class DeLayout : public KeyboardLayout {

public:
    /**
     * Default Constructor.
     */
    DeLayout() : KeyboardLayout() {
        uint8_t normalTab[89] = {
                0, 0, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', 225, 39, '\b',
                '\t', 'q', 'w', 'e', 'r', 't', 'z', 'u', 'i', 'o', 'p', 129, '+', '\n',
                0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', 148, 132, '^', 0, '#',
                'y', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '-', 0,
                '*', 0, ' ', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '-',
                0, 0, 0, '+', 0, 0, 0, 0, 0, 0, 0, '<', 0, 0
        };

        uint8_t shiftTab[89] = {
                0, 0, '!', '"', 21, '$', '%', '&', '/', '(', ')', '=', '?', 96, 0,
                0, 'Q', 'W', 'E', 'R', 'T', 'Z', 'U', 'I', 'O', 'P', 154, '*', 0,
                0, 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', 153, 142, 248, 0, 39,
                'Y', 'X', 'C', 'V', 'B', 'N', 'M', ';', ':', '_', 0,
                0, 0, ' ', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '>', 0, 0
        };

        uint8_t altTab[89] = {
                0, 0, 0, 253, 0, 0, 0, 0, '{', '[', ']', '}', '\\', 0, 0,
                0, '@', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '~', 0,
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 230, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '|', 0, 0
        };

        uint8_t asciiNumTab[13] = {
                '7', '8', '9', '-', '4', '5', '6', '+', '1', '2', '3', '0', ','
        };

        Util::Address<uint32_t>(KeyboardLayout::normalTab).copyRange(Util::Address<uint32_t>(normalTab), sizeof(KeyboardLayout::normalTab));
        Util::Address<uint32_t>(KeyboardLayout::shiftTab).copyRange(Util::Address<uint32_t>(shiftTab), sizeof(KeyboardLayout::shiftTab));
        Util::Address<uint32_t>(KeyboardLayout::altTab).copyRange(Util::Address<uint32_t>(altTab), sizeof(KeyboardLayout::altTab));
        Util::Address<uint32_t>(KeyboardLayout::asciiNumTab).copyRange(Util::Address<uint32_t>(asciiNumTab), sizeof(KeyboardLayout::asciiNumTab));
    }

    /**
     * Copy Constructor.
     */
    DeLayout(const DeLayout &other) = delete;

    /**
     * Assignment operator.
     */
    DeLayout &operator=(const DeLayout &other) = delete;

    /**
     * Destructor.
     */
    ~DeLayout() = default;

private:
};


}

#endif
