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

#include "palettes.h"

#include <util/graphic/Colors.h>

extern uint8_t gb_colour_hash(gb_s*);
extern const char* gb_get_rom_name(gb_s*, char*);

constexpr uint16_t MANUAL_PALETTES[14] {
        0x1d << 8 | 0x05, // Black and White
        0x1e << 8 | 0x05, // Shades of green
        0x12 << 8 | 0x00, // Up
        0x10 << 8 | 0x05, // A + Up
        0x19 << 8 | 0x03, // B + Up
        0x18 << 8 | 0x05, // Left
        0x0d << 8 | 0x05, // A + Left
        0x16 << 8 | 0x00, // B + Left
        0x17 << 8 | 0x00, // Down
        0x07 << 8 | 0x00, // A + Down
        0x1a << 8 | 0x05, // B + Down
        0x05 << 8 | 0x00, // Right
        0x1c << 8 | 0x03, // A + Right
        0x13 << 8 | 0x00 // B + Right
};

const Util::Graphic::Color CGB_PALLETES[31][3][4] {
        {
                { Util::Graphic::Color::fromRGB32(0xFFFFFF), Util::Graphic::Color::fromRGB32(0xFF7300), Util::Graphic::Color::fromRGB32(0x944200), Util::Graphic::Color::fromRGB32(0x000000) }, // OBJ0
                { Util::Graphic::Color::fromRGB32(0xFFFFFF), Util::Graphic::Color::fromRGB32(0x5ABDFF), Util::Graphic::Color::fromRGB32(0xFF0000), Util::Graphic::Color::fromRGB32(0x0000FF) }, // OBJ1
                { Util::Graphic::Color::fromRGB32(0xFFFFFF), Util::Graphic::Color::fromRGB32(0xADAD84), Util::Graphic::Color::fromRGB32(0x42737B), Util::Graphic::Color::fromRGB32(0x000000) }  // BG
        },
        {
                { Util::Graphic::Color::fromRGB32(0xFFC542), Util::Graphic::Color::fromRGB32(0xFFD600), Util::Graphic::Color::fromRGB32(0x943A00), Util::Graphic::Color::fromRGB32(0x4A0000) }, // OBJ0
                { Util::Graphic::Color::fromRGB32(0xFFFFFF), Util::Graphic::Color::fromRGB32(0xFF8484), Util::Graphic::Color::fromRGB32(0x943A3A), Util::Graphic::Color::fromRGB32(0x000000) }, // OBJ1
                { Util::Graphic::Color::fromRGB32(0xFFFF9C), Util::Graphic::Color::fromRGB32(0x94B5FF), Util::Graphic::Color::fromRGB32(0x639473), Util::Graphic::Color::fromRGB32(0x003A3A) }  // BG
        },
        {
                { Util::Graphic::Color::fromRGB32(0xFFFFFF), Util::Graphic::Color::fromRGB32(0xFFFFFF), Util::Graphic::Color::fromRGB32(0x63A5FF), Util::Graphic::Color::fromRGB32(0x0000FF) }, // OBJ0
                { Util::Graphic::Color::fromRGB32(0xFFFFFF), Util::Graphic::Color::fromRGB32(0xFFAD63), Util::Graphic::Color::fromRGB32(0x843100), Util::Graphic::Color::fromRGB32(0x000000) }, // OBJ1
                { Util::Graphic::Color::fromRGB32(0x6BFF00), Util::Graphic::Color::fromRGB32(0xFFFFFF), Util::Graphic::Color::fromRGB32(0xFF524A), Util::Graphic::Color::fromRGB32(0x000000) }  // BG
        },
        {
                { Util::Graphic::Color::fromRGB32(0xFFFFFF), Util::Graphic::Color::fromRGB32(0xFFFFFF), Util::Graphic::Color::fromRGB32(0x63A5FF), Util::Graphic::Color::fromRGB32(0x0000FF) }, // OBJ0
                { Util::Graphic::Color::fromRGB32(0xFFFFFF), Util::Graphic::Color::fromRGB32(0xFF8484), Util::Graphic::Color::fromRGB32(0x943A3A), Util::Graphic::Color::fromRGB32(0x000000) }, // OBJ1
                { Util::Graphic::Color::fromRGB32(0x52DE00), Util::Graphic::Color::fromRGB32(0xFF8400), Util::Graphic::Color::fromRGB32(0xFFFF00), Util::Graphic::Color::fromRGB32(0xFFFFFF) }  // BG
        },
        {
                { Util::Graphic::Color::fromRGB32(0xFFFFFF), Util::Graphic::Color::fromRGB32(0xFF8484), Util::Graphic::Color::fromRGB32(0x943A3A), Util::Graphic::Color::fromRGB32(0x000000) }, // OBJ0
                { Util::Graphic::Color::fromRGB32(0xFFFFFF), Util::Graphic::Color::fromRGB32(0xFF8484), Util::Graphic::Color::fromRGB32(0x943A3A), Util::Graphic::Color::fromRGB32(0x000000) }, // OBJ1
                { Util::Graphic::Color::fromRGB32(0xFFFFFF), Util::Graphic::Color::fromRGB32(0x7BFF00), Util::Graphic::Color::fromRGB32(0xB57300), Util::Graphic::Color::fromRGB32(0x000000) }  // BG
        },
        {
                { Util::Graphic::Color::fromRGB32(0xFFFFFF), Util::Graphic::Color::fromRGB32(0xFF8484), Util::Graphic::Color::fromRGB32(0x943A3A), Util::Graphic::Color::fromRGB32(0x000000) }, // OBJ0
                { Util::Graphic::Color::fromRGB32(0xFFFFFF), Util::Graphic::Color::fromRGB32(0x5ABDFF), Util::Graphic::Color::fromRGB32(0xFF0000), Util::Graphic::Color::fromRGB32(0x0000FF) }, // OBJ1
                { Util::Graphic::Color::fromRGB32(0xFFFFFF), Util::Graphic::Color::fromRGB32(0x52FF00), Util::Graphic::Color::fromRGB32(0xFF4200), Util::Graphic::Color::fromRGB32(0x000000) }  // BG
        },
        {
                { Util::Graphic::Color::fromRGB32(0xFFFFFF), Util::Graphic::Color::fromRGB32(0xFF8484), Util::Graphic::Color::fromRGB32(0x943A3A), Util::Graphic::Color::fromRGB32(0x000000) }, // OBJ0
                { Util::Graphic::Color::fromRGB32(0xFFFFFF), Util::Graphic::Color::fromRGB32(0x5ABDFF), Util::Graphic::Color::fromRGB32(0xFF0000), Util::Graphic::Color::fromRGB32(0x0000FF) }, // OBJ1
                { Util::Graphic::Color::fromRGB32(0xFFFFFF), Util::Graphic::Color::fromRGB32(0xFF9C00), Util::Graphic::Color::fromRGB32(0xFF0000), Util::Graphic::Color::fromRGB32(0x000000) }  // BG
        },
        {
                { Util::Graphic::Color::fromRGB32(0xFFFFFF), Util::Graphic::Color::fromRGB32(0x63A5FF), Util::Graphic::Color::fromRGB32(0x0000FF), Util::Graphic::Color::fromRGB32(0x000000) }, // OBJ0
                { Util::Graphic::Color::fromRGB32(0xFFFFFF), Util::Graphic::Color::fromRGB32(0x5ABDFF), Util::Graphic::Color::fromRGB32(0xFF0000), Util::Graphic::Color::fromRGB32(0x0000FF) }, // OBJ1
                { Util::Graphic::Color::fromRGB32(0xFFFFFF), Util::Graphic::Color::fromRGB32(0xFFFF00), Util::Graphic::Color::fromRGB32(0xFF0000), Util::Graphic::Color::fromRGB32(0x000000) }  // BG
        },
        {
                { Util::Graphic::Color::fromRGB32(0xFF6352), Util::Graphic::Color::fromRGB32(0xD60000), Util::Graphic::Color::fromRGB32(0x630000), Util::Graphic::Color::fromRGB32(0x000000) }, // OBJ0
                { Util::Graphic::Color::fromRGB32(0x0000FF), Util::Graphic::Color::fromRGB32(0xFFFFFF), Util::Graphic::Color::fromRGB32(0xFFFF7B), Util::Graphic::Color::fromRGB32(0x0084FF) }, // OBJ1
                { Util::Graphic::Color::fromRGB32(0xA59CFF), Util::Graphic::Color::fromRGB32(0xFFFF00), Util::Graphic::Color::fromRGB32(0x006300), Util::Graphic::Color::fromRGB32(0x000000) }  // BG
        },
        {
                { Util::Graphic::Color::fromRGB32(0xFFFFFF), Util::Graphic::Color::fromRGB32(0xFF7300), Util::Graphic::Color::fromRGB32(0x944200), Util::Graphic::Color::fromRGB32(0x000000) }, // OBJ0
                { Util::Graphic::Color::fromRGB32(0xFFFFFF), Util::Graphic::Color::fromRGB32(0x63A5FF), Util::Graphic::Color::fromRGB32(0x0000FF), Util::Graphic::Color::fromRGB32(0x000000) }, // OBJ1
                { Util::Graphic::Color::fromRGB32(0xFFFFCE), Util::Graphic::Color::fromRGB32(0x63EFEF), Util::Graphic::Color::fromRGB32(0x9C8431), Util::Graphic::Color::fromRGB32(0x5A5A5A) }  // BG
        },
        {
                { Util::Graphic::Color::fromRGB32(0x000000), Util::Graphic::Color::fromRGB32(0xFFFFFF), Util::Graphic::Color::fromRGB32(0xFF8484), Util::Graphic::Color::fromRGB32(0x943A3A) }, // OBJ0
                { Util::Graphic::Color::fromRGB32(0x000000), Util::Graphic::Color::fromRGB32(0xFFFFFF), Util::Graphic::Color::fromRGB32(0xFF8484), Util::Graphic::Color::fromRGB32(0x943A3A) }, // OBJ1
                { Util::Graphic::Color::fromRGB32(0xB5B5FF), Util::Graphic::Color::fromRGB32(0xFFFF94), Util::Graphic::Color::fromRGB32(0xAD5A42), Util::Graphic::Color::fromRGB32(0x000000) }  // BG
        },
        {
                { Util::Graphic::Color::fromRGB32(0xFFFFFF), Util::Graphic::Color::fromRGB32(0xFF8484), Util::Graphic::Color::fromRGB32(0x943A3A), Util::Graphic::Color::fromRGB32(0x000000) }, // OBJ0
                { Util::Graphic::Color::fromRGB32(0xFFFFFF), Util::Graphic::Color::fromRGB32(0xFFFF7B), Util::Graphic::Color::fromRGB32(0x0084FF), Util::Graphic::Color::fromRGB32(0xFF0000) }, // OBJ1
                { Util::Graphic::Color::fromRGB32(0xFFFFFF), Util::Graphic::Color::fromRGB32(0x63A5FF), Util::Graphic::Color::fromRGB32(0x0000FF), Util::Graphic::Color::fromRGB32(0x000000) }  // BG
        },
        {
                { Util::Graphic::Color::fromRGB32(0xFFC542), Util::Graphic::Color::fromRGB32(0xFFD600), Util::Graphic::Color::fromRGB32(0x943A00), Util::Graphic::Color::fromRGB32(0x4A0000) }, // OBJ0
                { Util::Graphic::Color::fromRGB32(0xFFFFFF), Util::Graphic::Color::fromRGB32(0x5ABDFF), Util::Graphic::Color::fromRGB32(0xFF0000), Util::Graphic::Color::fromRGB32(0x0000FF) }, // OBJ1
                { Util::Graphic::Color::fromRGB32(0xFFFFFF), Util::Graphic::Color::fromRGB32(0x8C8CDE), Util::Graphic::Color::fromRGB32(0x52528C), Util::Graphic::Color::fromRGB32(0x000000) }  // BG
        },
        {
                { Util::Graphic::Color::fromRGB32(0xFFFFFF), Util::Graphic::Color::fromRGB32(0xFF8484), Util::Graphic::Color::fromRGB32(0x943A3A), Util::Graphic::Color::fromRGB32(0x000000) }, // OBJ0
                { Util::Graphic::Color::fromRGB32(0xFFFFFF), Util::Graphic::Color::fromRGB32(0xFFAD63), Util::Graphic::Color::fromRGB32(0x843100), Util::Graphic::Color::fromRGB32(0x000000) }, // OBJ1
                { Util::Graphic::Color::fromRGB32(0xFFFFFF), Util::Graphic::Color::fromRGB32(0x8C8CDE), Util::Graphic::Color::fromRGB32(0x52528C), Util::Graphic::Color::fromRGB32(0x000000) }  // BG
        },
        {
                { Util::Graphic::Color::fromRGB32(0xFFFFFF), Util::Graphic::Color::fromRGB32(0xFF8484), Util::Graphic::Color::fromRGB32(0x943A3A), Util::Graphic::Color::fromRGB32(0x000000) }, // OBJ0
                { Util::Graphic::Color::fromRGB32(0xFFFFFF), Util::Graphic::Color::fromRGB32(0x63A5FF), Util::Graphic::Color::fromRGB32(0x0000FF), Util::Graphic::Color::fromRGB32(0x000000) }, // OBJ1
                { Util::Graphic::Color::fromRGB32(0xFFFFFF), Util::Graphic::Color::fromRGB32(0x7BFF31), Util::Graphic::Color::fromRGB32(0x008400), Util::Graphic::Color::fromRGB32(0x000000) }  // BG
        },
        {
                { Util::Graphic::Color::fromRGB32(0xFFFFFF), Util::Graphic::Color::fromRGB32(0x63A5FF), Util::Graphic::Color::fromRGB32(0x0000FF), Util::Graphic::Color::fromRGB32(0x000000) }, // OBJ0
                { Util::Graphic::Color::fromRGB32(0xFFFFFF), Util::Graphic::Color::fromRGB32(0x7BFF31), Util::Graphic::Color::fromRGB32(0x008400), Util::Graphic::Color::fromRGB32(0x000000) }, // OBJ1
                { Util::Graphic::Color::fromRGB32(0xFFFFFF), Util::Graphic::Color::fromRGB32(0xFFAD63), Util::Graphic::Color::fromRGB32(0x843100), Util::Graphic::Color::fromRGB32(0x000000) }  // BG
        },
        {
                { Util::Graphic::Color::fromRGB32(0xFFFFFF), Util::Graphic::Color::fromRGB32(0x7BFF31), Util::Graphic::Color::fromRGB32(0x008400), Util::Graphic::Color::fromRGB32(0x000000) }, // OBJ0
                { Util::Graphic::Color::fromRGB32(0xFFFFFF), Util::Graphic::Color::fromRGB32(0xFF8484), Util::Graphic::Color::fromRGB32(0x943A3A), Util::Graphic::Color::fromRGB32(0x000000) }, // OBJ1
                { Util::Graphic::Color::fromRGB32(0xFFFFFF), Util::Graphic::Color::fromRGB32(0xFF8484), Util::Graphic::Color::fromRGB32(0x943A3A), Util::Graphic::Color::fromRGB32(0x000000) }  // BG
        },
        {
                { Util::Graphic::Color::fromRGB32(0xFFFFFF), Util::Graphic::Color::fromRGB32(0x00FF00), Util::Graphic::Color::fromRGB32(0x318400), Util::Graphic::Color::fromRGB32(0x004A00) }, // OBJ0
                { Util::Graphic::Color::fromRGB32(0xFFFFFF), Util::Graphic::Color::fromRGB32(0x63A5FF), Util::Graphic::Color::fromRGB32(0x0000FF), Util::Graphic::Color::fromRGB32(0x000000) }, // OBJ1
                { Util::Graphic::Color::fromRGB32(0xFFFFFF), Util::Graphic::Color::fromRGB32(0xFF8484), Util::Graphic::Color::fromRGB32(0x943A3A), Util::Graphic::Color::fromRGB32(0x000000) }  // BG
        },
        {
                { Util::Graphic::Color::fromRGB32(0xFFFFFF), Util::Graphic::Color::fromRGB32(0x7BFF31), Util::Graphic::Color::fromRGB32(0x008400), Util::Graphic::Color::fromRGB32(0x004A00) }, // OBJ0
                { Util::Graphic::Color::fromRGB32(0xFFFFFF), Util::Graphic::Color::fromRGB32(0x63A5FF), Util::Graphic::Color::fromRGB32(0x0000FF), Util::Graphic::Color::fromRGB32(0x000000) }, // OBJ1
                { Util::Graphic::Color::fromRGB32(0xFFFFFF), Util::Graphic::Color::fromRGB32(0xFFAD63), Util::Graphic::Color::fromRGB32(0x843100), Util::Graphic::Color::fromRGB32(0x000000) }  // BG
        },
        {
                { Util::Graphic::Color::fromRGB32(0xFFFFFF), Util::Graphic::Color::fromRGB32(0xFF8484), Util::Graphic::Color::fromRGB32(0x943A3A), Util::Graphic::Color::fromRGB32(0x000000) }, // OBJ0
                { Util::Graphic::Color::fromRGB32(0xFFFFFF), Util::Graphic::Color::fromRGB32(0x7BFF31), Util::Graphic::Color::fromRGB32(0x008400), Util::Graphic::Color::fromRGB32(0x000000) }, // OBJ1
                { Util::Graphic::Color::fromRGB32(0x000000), Util::Graphic::Color::fromRGB32(0x008484), Util::Graphic::Color::fromRGB32(0xFFDE00), Util::Graphic::Color::fromRGB32(0xFFFFFF) }  // BG
        },
        {
                { Util::Graphic::Color::fromRGB32(0xFFFF00), Util::Graphic::Color::fromRGB32(0xFF0000), Util::Graphic::Color::fromRGB32(0x630000), Util::Graphic::Color::fromRGB32(0x000000) }, // OBJ0
                { Util::Graphic::Color::fromRGB32(0xFFFFFF), Util::Graphic::Color::fromRGB32(0x7BFF31), Util::Graphic::Color::fromRGB32(0x008400), Util::Graphic::Color::fromRGB32(0x000000) }, // OBJ1
                { Util::Graphic::Color::fromRGB32(0xFFFFFF), Util::Graphic::Color::fromRGB32(0x63A5FF), Util::Graphic::Color::fromRGB32(0x0000FF), Util::Graphic::Color::fromRGB32(0x000000) }  // BG
        },
        {
                { Util::Graphic::Color::fromRGB32(0xFFFF00), Util::Graphic::Color::fromRGB32(0xFF0000), Util::Graphic::Color::fromRGB32(0x630000), Util::Graphic::Color::fromRGB32(0x000000) }, // OBJ0
                { Util::Graphic::Color::fromRGB32(0xFFFFFF), Util::Graphic::Color::fromRGB32(0x7BFF31), Util::Graphic::Color::fromRGB32(0x008400), Util::Graphic::Color::fromRGB32(0x000000) }, // OBJ1
                { Util::Graphic::Color::fromRGB32(0xFFFFFF), Util::Graphic::Color::fromRGB32(0x63A5FF), Util::Graphic::Color::fromRGB32(0x0000FF), Util::Graphic::Color::fromRGB32(0x000000) }  // BG
        },
        {
                { Util::Graphic::Color::fromRGB32(0xFFFFFF), Util::Graphic::Color::fromRGB32(0xA5A5A5), Util::Graphic::Color::fromRGB32(0x525252), Util::Graphic::Color::fromRGB32(0x000000) }, // OBJ0
                { Util::Graphic::Color::fromRGB32(0xFFFFFF), Util::Graphic::Color::fromRGB32(0xA5A5A5), Util::Graphic::Color::fromRGB32(0x525252), Util::Graphic::Color::fromRGB32(0x000000) }, // OBJ1
                { Util::Graphic::Color::fromRGB32(0xFFFFFF), Util::Graphic::Color::fromRGB32(0xA5A5A5), Util::Graphic::Color::fromRGB32(0x525252), Util::Graphic::Color::fromRGB32(0x000000) }  // BG
        },
        {
                { Util::Graphic::Color::fromRGB32(0xFFFFA5), Util::Graphic::Color::fromRGB32(0xFF9494), Util::Graphic::Color::fromRGB32(0x9494FF), Util::Graphic::Color::fromRGB32(0x000000) }, // OBJ0
                { Util::Graphic::Color::fromRGB32(0xFFFFA5), Util::Graphic::Color::fromRGB32(0xFF9494), Util::Graphic::Color::fromRGB32(0x9494FF), Util::Graphic::Color::fromRGB32(0x000000) }, // OBJ1
                { Util::Graphic::Color::fromRGB32(0xFFFFA5), Util::Graphic::Color::fromRGB32(0xFF9494), Util::Graphic::Color::fromRGB32(0x9494FF), Util::Graphic::Color::fromRGB32(0x000000) }  // BG
        },
        {
                { Util::Graphic::Color::fromRGB32(0xFFFFFF), Util::Graphic::Color::fromRGB32(0xFF8484), Util::Graphic::Color::fromRGB32(0x943A3A), Util::Graphic::Color::fromRGB32(0x000000) }, // OBJ0
                { Util::Graphic::Color::fromRGB32(0xFFFFFF), Util::Graphic::Color::fromRGB32(0x7BFF31), Util::Graphic::Color::fromRGB32(0x008400), Util::Graphic::Color::fromRGB32(0x000000) }, // OBJ1
                { Util::Graphic::Color::fromRGB32(0xFFFFFF), Util::Graphic::Color::fromRGB32(0x63A5FF), Util::Graphic::Color::fromRGB32(0x0000FF), Util::Graphic::Color::fromRGB32(0x000000) }  // BG
        },
        {
                { Util::Graphic::Color::fromRGB32(0xFFFFFF), Util::Graphic::Color::fromRGB32(0xFFAD63), Util::Graphic::Color::fromRGB32(0x843100), Util::Graphic::Color::fromRGB32(0x000000) }, // OBJ0
                { Util::Graphic::Color::fromRGB32(0xFFFFFF), Util::Graphic::Color::fromRGB32(0xFFAD63), Util::Graphic::Color::fromRGB32(0x843100), Util::Graphic::Color::fromRGB32(0x000000) }, // OBJ1
                { Util::Graphic::Color::fromRGB32(0xFFE6C5), Util::Graphic::Color::fromRGB32(0xCE9C84), Util::Graphic::Color::fromRGB32(0x846B29), Util::Graphic::Color::fromRGB32(0x5A3108) }  // BG
        },
        {
                { Util::Graphic::Color::fromRGB32(0xFFFFFF), Util::Graphic::Color::fromRGB32(0x63A5FF), Util::Graphic::Color::fromRGB32(0x0000FF), Util::Graphic::Color::fromRGB32(0x000000) }, // OBJ0
                { Util::Graphic::Color::fromRGB32(0xFFFFFF), Util::Graphic::Color::fromRGB32(0x7BFF31), Util::Graphic::Color::fromRGB32(0x008400), Util::Graphic::Color::fromRGB32(0x000000) }, // OBJ1
                { Util::Graphic::Color::fromRGB32(0xFFFFFF), Util::Graphic::Color::fromRGB32(0xFFFF00), Util::Graphic::Color::fromRGB32(0x7B4A00), Util::Graphic::Color::fromRGB32(0x000000) }  // BG
        },
        {
                { Util::Graphic::Color::fromRGB32(0xFFFFFF), Util::Graphic::Color::fromRGB32(0xFFCE00), Util::Graphic::Color::fromRGB32(0x9C6300), Util::Graphic::Color::fromRGB32(0x000000) }, // OBJ0
                { Util::Graphic::Color::fromRGB32(0xFFFFFF), Util::Graphic::Color::fromRGB32(0xFFCE00), Util::Graphic::Color::fromRGB32(0x9C6300), Util::Graphic::Color::fromRGB32(0x000000) }, // OBJ1
                { Util::Graphic::Color::fromRGB32(0xFFFFFF), Util::Graphic::Color::fromRGB32(0xFFCE00), Util::Graphic::Color::fromRGB32(0x9C6300), Util::Graphic::Color::fromRGB32(0x000000) }  // BG
        },
        {
                { Util::Graphic::Color::fromRGB32(0xFFFFFF), Util::Graphic::Color::fromRGB32(0xFF8484), Util::Graphic::Color::fromRGB32(0x943A3A), Util::Graphic::Color::fromRGB32(0x000000) }, // OBJ0
                { Util::Graphic::Color::fromRGB32(0xFFFFFF), Util::Graphic::Color::fromRGB32(0x63A5FF), Util::Graphic::Color::fromRGB32(0x0000FF), Util::Graphic::Color::fromRGB32(0x000000) }, // OBJ1
                { Util::Graphic::Color::fromRGB32(0xFFFFFF), Util::Graphic::Color::fromRGB32(0x7BFF31), Util::Graphic::Color::fromRGB32(0x0063C5), Util::Graphic::Color::fromRGB32(0x000000) }  // BG
        },
        { // Black and White (not included in CGB boot rom)
                { Util::Graphic::Color::fromRGB32(0xFFFFFF), Util::Graphic::Color::fromRGB32(0xAAAAAA), Util::Graphic::Color::fromRGB32(0x555555), Util::Graphic::Color::fromRGB32(0x000000) }, // OBJ0
                { Util::Graphic::Color::fromRGB32(0xFFFFFF), Util::Graphic::Color::fromRGB32(0xAAAAAA), Util::Graphic::Color::fromRGB32(0x555555), Util::Graphic::Color::fromRGB32(0x000000) }, // OBJ1
                { Util::Graphic::Color::fromRGB32(0xFFFFFF), Util::Graphic::Color::fromRGB32(0xAAAAAA), Util::Graphic::Color::fromRGB32(0x555555), Util::Graphic::Color::fromRGB32(0x000000) }  // BG
        },
        { // Shades of green (not included in CGB boot rom)
                { Util::Graphic::Color::fromRGB32(0x9bbc0f), Util::Graphic::Color::fromRGB32(0x8bac0f), Util::Graphic::Color::fromRGB32(0x306230), Util::Graphic::Color::fromRGB32(0x0f380f) }, // OBJ0
                { Util::Graphic::Color::fromRGB32(0x9bbc0f), Util::Graphic::Color::fromRGB32(0x8bac0f), Util::Graphic::Color::fromRGB32(0x306230), Util::Graphic::Color::fromRGB32(0x0f380f) }, // OBJ1
                { Util::Graphic::Color::fromRGB32(0x9bbc0f), Util::Graphic::Color::fromRGB32(0x8bac0f), Util::Graphic::Color::fromRGB32(0x306230), Util::Graphic::Color::fromRGB32(0x0f380f) }  // BG
        }
};

uint32_t* getPalette(const uint8_t index, const uint8_t flags, const uint8_t colorDepth) {
    auto *palette = CGB_PALLETES[index];
    auto *ret = new uint32_t[3][4];

    for (uint8_t i = 0; i < 3; i++) {
        for (uint8_t j = 0; j < 4; j++) {
            ret[i][j] = palette[i][j].getColorForDepth(colorDepth);
        }
    }

    switch (flags) {
        case 0x00: // OBJ0 = BG, OBJ1 = BG
            ret[0][0] = ret[2][0];
            ret[0][1] = ret[2][1];
            ret[0][2] = ret[2][2];

            ret[1][0] = ret[2][0];
            ret[1][1] = ret[2][1];
            ret[1][2] = ret[2][2];
            break;
        case 0x01: // OBJ1 = BG
            ret[1][0] = ret[2][0];
            ret[1][1] = ret[2][1];
            ret[1][2] = ret[2][2];
            break;
        case 0x02: // OBJ1 = OBJ0, OBJ0 = BG
            ret[1][0] = ret[0][0];
            ret[1][1] = ret[0][1];
            ret[1][2] = ret[0][2];

            ret[0][0] = ret[2][0];
            ret[0][1] = ret[2][1];
            ret[0][2] = ret[2][2];
            break;
        case 0x03: // OBJ1 = OBJ0
            ret[1][0] = ret[0][0];
            ret[1][1] = ret[0][1];
            ret[1][2] = ret[0][2];
            break;
        case 0x04: // OBJ0 = BG
            ret[0][0] = ret[2][0];
            ret[0][1] = ret[2][1];
            ret[0][2] = ret[2][2];
            break;
        default:
            break;
    }

    return reinterpret_cast<uint32_t*>(ret);
}

uint32_t* getManualPalette(const uint8_t manualIndex, const uint8_t colorDepth) {
    const auto index = MANUAL_PALETTES[manualIndex] >> 8;
    const auto flags = MANUAL_PALETTES[manualIndex] & 0xff;

    return getPalette(index, flags, colorDepth);
}

uint32_t* getPaletteForRom(const char *romTitle, const uint8_t colorHash, const uint8_t colorDepth) {
    switch (colorHash) {
        case 0xb3: {
            if (romTitle[3] == 'U') {
                return getPaletteForRom(0x00, 0x03, colorDepth);
            }
            if (romTitle[3] == 'R') {
                return getPalette(0x05, 0x04, colorDepth);
            }
            if (romTitle[3] == 'B') {
                return getPalette(0x08, 0x05, colorDepth);
            }
        }
        break;
        case 0x59:
            return getPaletteForRom(0x00, 0x05, colorDepth);
        case 0xc6: {
            if (romTitle[3] == 'A') {
                return getPaletteForRom(0x00, 0x05, colorDepth);
            }
            if (romTitle[3] == ' ') {
                return getPalette(0x1c, 0x03, colorDepth);
            }
        }
        break;
        case 0x8c:
            return getPalette(0x01, 0x00, colorDepth);
        case 0x86:
        case 0xa8:
            return getPalette(0x01, 0x05, colorDepth);
        case 0xbf:
        if (romTitle[3] == 'C') {
            return getPalette(0x02, 0x05, colorDepth);
        } else
        if (romTitle[3] == ' ') {
            return getPalette(0x0d, 0x03, colorDepth);
        }
        break;
        case 0xce:
        case 0xd1:
        case 0xf0:
            return getPalette(0x02, 0x05, colorDepth);
        case 0x36:
            return getPalette(0x03, 0x05, colorDepth);
        case 0x34:
            return getPalette(0x04, 0x03, colorDepth);
        case 0x66: {
            if (romTitle[3] == 'E') {
                return getPalette(0x04, 0x03, colorDepth);
            }
            if (romTitle[3] == 'L') {
                return getPalette(0x1c, 0x03, colorDepth);
            }
        }
        break;
        case 0xf4: {
            if (romTitle[3] == ' ') {
                return getPalette(0x04, 0x03, colorDepth);
            }
            if (romTitle[3] == '-') {
                return getPalette(0x1c, 0x05, colorDepth);
            }
        }
        break;
        case 0x3d:
            return getPalette(0x05, 0x03, colorDepth);
        case 0x6a: {
            if (romTitle[3] == 'I') {
                return getPalette(0x05, 0x03, colorDepth);
            }
            if (romTitle[3] == 'K') {
                return getPalette(0x0c, 0x05, colorDepth);
            }
        }
        break;
        case 0x95:
            return getPalette(0x05, 0x04, colorDepth);
        case 0x71:
        case 0xff:
            return getPalette(0x06, 0x00, colorDepth);
        case 0x19:
            return getPalette(0x06, 0x03, colorDepth);
        case 0x3e:
        case 0xe0:
            return getPalette(0x06, 0x04, colorDepth);
        case 0x15:
        case 0xdb:
            return getPalette(0x07, 0x00, colorDepth);
        case 0x0d: {
            if (romTitle[3] == 'R') {
                return getPalette(0x07, 0x04, colorDepth);
            }
            if (romTitle[3] == 'E') {
                return getPalette(0x0c, 0x03, colorDepth);
            }
        }
        break;
        case 0x69:
        case 0xf2:
            return getPalette(0x07, 0x04, colorDepth);
        case 0x88:
            return getPalette(0x08, 0x00, colorDepth);
        case 0x1d:
            return getPalette(0x08, 0x03, colorDepth);
        case 0x27: {
            if (romTitle[3] == 'B') {
                return getPalette(0x08, 0x05, colorDepth);
            }
            if (romTitle[3] == 'N') {
                return getPalette(0x0e, 0x05, colorDepth);
            }
        }
        break;
        case 0x49:
        case 0x5c:
            return getPalette(0x08, 0x05, colorDepth);
        case 0xc9:
            return getPalette(0x09, 0x05, colorDepth);
        case 0x46: {
            if (romTitle[3] == 'E') {
                return getPalette(0x0a, 0x03, colorDepth);
            }
            if (romTitle[3] == 'R') {
                return getPalette(0x14, 0x05, colorDepth);
            }
        }
        break;
        case 0x61: {
            if (romTitle[3] == 'E') {
                return getPalette(0x0b, 0x01, colorDepth);
            }
            if (romTitle[3] == 'A') {
                return getPalette(0x0e, 0x05, colorDepth);
            }
        }
        break;
        case 0x3c:
            return getPalette(0x0b, 0x02, colorDepth);
        case 0x4e:
            return getPalette(0x0b, 0x05, colorDepth);
        case 0x9c:
            return getPalette(0x0c, 0x02, colorDepth);
        case 0x18: {
            if (romTitle[3] == 'K') {
                return getPalette(0x0c, 0x05, colorDepth);
            }
            if (romTitle[3] == 'I') {
                return getPalette(0x1c, 0x03, colorDepth);
            }
        }
        break;
        case 0x6b:
            return getPalette(0x0c, 0x05, colorDepth);
        case 0xd3: {
            if (romTitle[3] == 'R') {
                return getPalette(0x0d, 0x01, colorDepth);
            }
            if (romTitle[3] == 'I') {
                return getPalette(0x15, 0x05, colorDepth);
            }
        }
        break;
        case 0x9d:
            return getPalette(0x0d, 0x05, colorDepth);
        case 0x28: {
            if (romTitle[3] == 'F') {
                return getPalette(0x0e, 0x03, colorDepth);
            }
            if (romTitle[3] == 'A') {
                return getPalette(0x13, 0x00, colorDepth);
            }
        }
        break;
        case 0x4b:
        case 0x90:
        case 0x9a:
        case 0xbd:
            return getPalette(0x0e, 0x03, colorDepth);
        case 0x17:
        case 0x8b:
            return getPalette(0x0e, 0x05, colorDepth);
        case 0x39:
        case 0x43:
        case 0x97:
            return getPalette(0x0f, 0x03, colorDepth);
        case 0x01:
        case 0x10:
        case 0x29:
        case 0x52:
        case 0x5d:
        case 0x68:
        case 0x6d:
        case 0xf6:
            return getPalette(0x0f, 0x05, colorDepth);
        case 0x14:
            return getPalette(0x10, 0x01, colorDepth);
        case 0x70:
            return getPalette(0x11, 0x05, colorDepth);
        case 0x0c:
        case 0x16:
        case 0x35:
        case 0x67:
        case 0x75:
        case 0x92:
        case 0x99:
        case 0xb7:
            return getPalette(0x12, 0x00, colorDepth);
        case 0xa5: {
            if (romTitle[3] == 'R') {
                return getPalette(0x12, 0x03, colorDepth);
            }
            if (romTitle[3] == 'A') {
                return getPalette(0x13, 0x00, colorDepth);
            }
        }
        break;
        case 0xa2:
        case 0xf7:
            return getPalette(0x12, 0x05, colorDepth);
        case 0xe8:
            return getPalette(0x13, 0x00, colorDepth);
        case 0x58:
            return getPalette(0x16, 0x00, colorDepth);
        case 0x6f:
            return getPalette(0x1b, 0x00, colorDepth);
        case 0xaa:
            return getPalette(0x1c, 0x01, colorDepth);
        case 0x00:
        case 0x3f:
            return getPalette(0x1c, 0x03, colorDepth);
        default:
            break;
    }

    return getPalette(0x1c, 0x03, colorDepth);
}

uint32_t * getPalette(gb_s *gb, const uint8_t colorDepth) {
    char title[16]{};
    gb_get_rom_name(gb, title);
    const auto hash = gb_colour_hash(gb);

    return getPaletteForRom(title, hash, colorDepth);
}
