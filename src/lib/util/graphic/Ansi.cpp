/*
 * Copyright (C) 2018-2021 Heinrich-Heine-Universitaet Duesseldorf,
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

#include "Ansi.h"

namespace Util::Graphic {

const Color Ansi::colorTable256[256] = {
        // 16 predefined colors, matching the 4-bit ANSI colors
        Colors::BLACK, Colors::RED, Colors::GREEN, Colors::YELLOW,
        Colors::BLUE, Colors::MAGENTA, Colors::CYAN, Colors::WHITE,
        Colors::BLACK.bright(), Colors::RED.bright(), Colors::GREEN.bright(), Colors::YELLOW.bright(),
        Colors::BLUE.bright(), Colors::MAGENTA.bright(), Colors::CYAN.bright(), Colors::WHITE.bright(),

        // 216 colors
        Color(0, 0, 0), Color(0, 0, 95), Color(0, 0, 135),
        Color(0, 0, 175), Color(0, 0, 215), Color(0, 0, 255),

        Color(0, 95, 0), Color(0, 95, 95), Color(0, 95, 135),
        Color(0, 95, 175), Color(0, 95, 215), Color(0, 95, 255),

        Color(0, 135, 0), Color(0, 135, 95), Color(0, 135, 135),
        Color(0, 135, 175), Color(0, 135, 215), Color(0, 135, 255),

        Color(0, 175, 0), Color(0, 175, 95), Color(0, 175, 135),
        Color(0, 175, 175), Color(0, 175, 215), Color(0, 175, 255),

        Color(0, 215, 0), Color(0, 215, 95), Color(0, 215, 135),
        Color(0, 215, 175), Color(0, 215, 215), Color(0, 215, 255),

        Color(0, 255, 0), Color(0, 255, 95), Color(0, 255, 135),
        Color(0, 255, 175), Color(0, 255, 215), Color(0, 255, 255),

        Color(95, 0, 0), Color(95, 0, 95), Color(95, 0, 135),
        Color(95, 0, 175), Color(95, 0, 215), Color(95, 0, 255),

        Color(95, 95, 0), Color(95, 95, 95), Color(95, 95, 135),
        Color(95, 95, 175), Color(95, 95, 215), Color(95, 95, 255),

        Color(95, 135, 0), Color(95, 135, 95), Color(95, 135, 135),
        Color(95, 135, 175), Color(95, 135, 215), Color(95, 135, 255),

        Color(95, 175, 0), Color(95, 175, 95), Color(95, 175, 135),
        Color(95, 175, 175), Color(95, 175, 215), Color(95, 175, 255),

        Color(95, 215, 0), Color(95, 215, 95), Color(95, 215, 135),
        Color(95, 215, 175), Color(95, 215, 215), Color(95, 215, 255),

        Color(95, 255, 0), Color(95, 255, 95), Color(95, 255, 135),
        Color(95, 255, 175), Color(95, 255, 215), Color(95, 255, 255),

        Color(135, 0, 0), Color(135, 0, 95), Color(135, 0, 135),
        Color(135, 0, 175), Color(135, 0, 215), Color(135, 0, 255),

        Color(135, 95, 0), Color(135, 95, 95), Color(135, 95, 135),
        Color(135, 95, 175), Color(135, 95, 215), Color(135, 95, 255),

        Color(135, 135, 0), Color(135, 135, 95), Color(135, 135, 135),
        Color(135, 135, 175), Color(135, 135, 215), Color(135, 135, 255),

        Color(135, 175, 0), Color(135, 175, 95), Color(135, 175, 135),
        Color(135, 175, 175), Color(135, 175, 215), Color(135, 175, 255),

        Color(135, 215, 0), Color(135, 215, 95), Color(135, 215, 135),
        Color(135, 215, 175), Color(135, 215, 215), Color(135, 215, 255),

        Color(135, 255, 0), Color(135, 255, 95), Color(135, 255, 135),
        Color(135, 255, 175), Color(135, 255, 215), Color(135, 255, 255),

        Color(175, 0, 0), Color(175, 0, 95), Color(175, 0, 135),
        Color(175, 0, 175), Color(175, 0, 215), Color(175, 0, 255),

        Color(175, 95, 0), Color(175, 95, 95), Color(175, 95, 135),
        Color(175, 95, 175), Color(175, 95, 215), Color(175, 95, 255),

        Color(175, 135, 0), Color(175, 135, 95), Color(175, 135, 135),
        Color(175, 135, 175), Color(175, 135, 215), Color(175, 135, 255),

        Color(175, 175, 0), Color(175, 175, 95), Color(175, 175, 135),
        Color(175, 175, 175), Color(175, 175, 215), Color(175, 175, 255),

        Color(175, 215, 0), Color(175, 215, 95), Color(175, 215, 135),
        Color(175, 215, 175), Color(175, 215, 215), Color(175, 215, 255),

        Color(175, 255, 0), Color(175, 255, 95), Color(175, 255, 135),
        Color(175, 255, 175), Color(175, 255, 215), Color(175, 255, 255),

        Color(215, 0, 0), Color(215, 0, 95), Color(215, 0, 135),
        Color(215, 0, 175), Color(215, 0, 215), Color(215, 0, 255),

        Color(215, 95, 0), Color(215, 95, 95), Color(215, 95, 135),
        Color(215, 95, 175), Color(215, 95, 215), Color(215, 95, 255),

        Color(215, 135, 0), Color(215, 135, 95), Color(215, 135, 135),
        Color(215, 135, 175), Color(215, 135, 215), Color(215, 135, 255),

        Color(215, 175, 0), Color(215, 175, 95), Color(215, 175, 135),
        Color(215, 175, 175), Color(215, 175, 215), Color(215, 175, 255),

        Color(215, 215, 0), Color(215, 215, 95), Color(215, 215, 135),
        Color(215, 215, 175), Color(215, 215, 215), Color(215, 215, 255),

        Color(215, 255, 0), Color(215, 255, 95), Color(215, 255, 135),
        Color(215, 255, 175), Color(215, 255, 215), Color(215, 255, 255),

        Color(255, 0, 0), Color(255, 0, 95), Color(255, 0, 135),
        Color(255, 0, 175), Color(255, 0, 215), Color(255, 0, 255),

        Color(255, 95, 0), Color(255, 95, 95), Color(255, 95, 135),
        Color(255, 95, 175), Color(255, 95, 215), Color(255, 95, 255),

        Color(255, 135, 0), Color(255, 135, 95), Color(255, 135, 135),
        Color(255, 135, 175), Color(255, 135, 215), Color(255, 135, 255),

        Color(255, 175, 0), Color(255, 175, 95), Color(255, 175, 135),
        Color(255, 175, 175), Color(255, 175, 215), Color(255, 175, 255),

        Color(255, 215, 0), Color(255, 215, 95), Color(255, 215, 135),
        Color(255, 215, 175), Color(255, 215, 215), Color(255, 215, 255),

        Color(255, 255, 0), Color(255, 255, 95), Color(255, 255, 135),
        Color(255, 255, 175), Color(255, 255, 215), Color(255, 255, 255),

        // 24 grayscale colors
        Color(8, 8, 8), Color(18, 18, 18), Color(28, 28, 28),
        Color(38, 38, 8), Color(48, 48, 48), Color(58, 58, 58),
        Color(68, 68, 68), Color(78, 78, 78), Color(88, 88, 88),
        Color(98, 98, 98), Color(108, 108, 108), Color(118, 118, 118),
        Color(128, 128, 128), Color(138, 138, 138), Color(148, 148, 148),
        Color(158, 158, 158), Color(168, 168, 168), Color(178, 178, 178),
        Color(188, 188, 188), Color(198, 198, 198), Color(208, 208, 208),
        Color(218, 218, 218), Color(228, 228, 228), Color(238, 238, 238)
};

Color Ansi::get8BitColor(uint8_t index) {
    return colorTable256[index];
}

Memory::String Ansi::foreground8BitColor(uint8_t colorIndex) {
    return Memory::String::format("\u001b[38;5;%um", colorIndex);
}

Memory::String Ansi::background8BitColor(uint8_t colorIndex) {
    return Memory::String::format("\u001b[48;5;%um", colorIndex);
}

Memory::String Ansi::foreground24BitColor(const Color &color) {
    return Memory::String::format("\u001b[38;2;%u;%u;%um", color.getRed(), color.getGreen(), color.getBlue());
}

Memory::String Ansi::background24BitColor(const Color &color) {
    return Memory::String::format("\u001b[48;2;%u;%u;%um", color.getRed(), color.getGreen(), color.getBlue());
}

}