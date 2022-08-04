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

#ifndef __Ansi_include__
#define __Ansi_include__

#include "lib/util/memory/String.h"
#include "Color.h"
#include "Colors.h"

namespace Util::Graphic {

class Ansi {

public:

    enum GraphicRendition {
        NORMAL = 0,
        BRIGHT = 1,
        DIM = 2,
        ITALIC = 3,
        UNDERLINE = 4,
        SLOW_BLINK = 5,
        FAST_BLINK = 6,
        INVERT = 7,
        RESET_BRIGHT_DIM = 22,
        RESET_ITALIC = 23,
        RESET_UNDERLINE = 24,
        RESET_BLINK = 25,
        RESET_INVERT = 27
    };


    /**
     * Default Constructor.
     * Deleted, as this class has only static members.
     */
    Ansi() = delete;

    /**
     * Copy Constructor.
     */
    Ansi(const Ansi &other) = delete;

    /**
     * Assignment operator.
     */
    Ansi &operator=(const Ansi &other) = delete;

    /**
     * Destructor.
     */
    ~Ansi() = default;

    static Color get8BitColor(uint8_t index);

    static Memory::String foreground8BitColor(uint8_t colorIndex);

    static Memory::String background8BitColor(uint8_t colorIndex);

    static Memory::String foreground24BitColor(const Color &color);

    static Memory::String background24BitColor(const Color &color);

    static const constexpr char *RESET = "\u001b[0m";
    static const constexpr char *BLACK = "\u001b[30m";
    static const constexpr char *RED = "\u001b[31m";
    static const constexpr char *GREEN = "\u001b[32m";
    static const constexpr char *YELLOW = "\u001b[33m";
    static const constexpr char *BLUE = "\u001b[34m";
    static const constexpr char *MAGENTA = "\u001b[35m";
    static const constexpr char *CYAN = "\u001b[36m";
    static const constexpr char *WHITE = "\u001b[37m";
    static const constexpr char *BRIGHT_BLACK = "\u001b[90m";
    static const constexpr char *BRIGHT_RED = "\u001b[91m";
    static const constexpr char *BRIGHT_GREEN = "\u001b[92m";
    static const constexpr char *BRIGHT_YELLOW = "\u001b[93m";
    static const constexpr char *BRIGHT_BLUE = "\u001b[94m";
    static const constexpr char *BRIGHT_MAGENTA = "\u001b[95m";
    static const constexpr char *BRIGHT_CYAN = "\u001b[96m";
    static const constexpr char *BRIGHT_WHITE = "\u001b[97m";
    static const constexpr char *BACKGROUND_BLACK = "\u001b[40m";
    static const constexpr char *BACKGROUND_RED = "\u001b[41m";
    static const constexpr char *BACKGROUND_GREEN = "\u001b[42m";
    static const constexpr char *BACKGROUND_YELLOW = "\u001b[43m";
    static const constexpr char *BACKGROUND_BLUE = "\u001b[44m";
    static const constexpr char *BACKGROUND_MAGENTA = "\u001b[45m";
    static const constexpr char *BACKGROUND_CYAN = "\u001b[46m";
    static const constexpr char *BACKGROUND_WHITE = "\u001b[47m";
    static const constexpr char *BACKGROUND_BRIGHT_BLACK = "\u001b[100m";
    static const constexpr char *BACKGROUND_BRIGHT_RED = "\u001b[101m";
    static const constexpr char *BACKGROUND_BRIGHT_GREEN = "\u001b[102m";
    static const constexpr char *BACKGROUND_BRIGHT_YELLOW = "\u001b[103m";
    static const constexpr char *BACKGROUND_BRIGHT_BLUE = "\u001b[104m";
    static const constexpr char *BACKGROUND_BRIGHT_MAGENTA = "\u001b[105m";
    static const constexpr char *BACKGROUND_BRIGHT_CYAN = "\u001b[106m";
    static const constexpr char *BACKGROUND_BRIGHT_WHITE = "\u001b[107m";

    static const constexpr char *ERASE_SCREEN_FROM_CURSOR = "\u001b[0J";
    static const constexpr char *ERASE_SCREEN_TO_CURSOR = "\u001b[1J";
    static const constexpr char *ERASE_SCREEN = "\u001b[2J";
    static const constexpr char *ERASE_LINE_FROM_CURSOR = "\u001b[0K";
    static const constexpr char *ERASE_LINE_TO_CURSOR = "\u001b[1K";
    static const constexpr char *ERASE_LINE = "\u001b[2K";

private:

    static const Graphic::Color colorTable256[256];

};

}

#endif
