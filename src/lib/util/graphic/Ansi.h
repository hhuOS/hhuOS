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

#ifndef __Ansi_include__
#define __Ansi_include__

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

    static constexpr const char *RESET = "\u001b[0m";
    static constexpr const char *BLACK = "\u001b[30m";
    static constexpr const char *RED = "\u001b[31m";
    static constexpr const char *GREEN = "\u001b[32m";
    static constexpr const char *YELLOW = "\u001b[33m";
    static constexpr const char *BLUE = "\u001b[34m";
    static constexpr const char *MAGENTA = "\u001b[35m";
    static constexpr const char *CYAN = "\u001b[36m";
    static constexpr const char *WHITE = "\u001b[37m";
    static constexpr const char *BRIGHT_BLACK = "\u001b[90m";
    static constexpr const char *BRIGHT_RED = "\u001b[91m";
    static constexpr const char *BRIGHT_GREEN = "\u001b[92m";
    static constexpr const char *BRIGHT_YELLOW = "\u001b[93m";
    static constexpr const char *BRIGHT_BLUE = "\u001b[94m";
    static constexpr const char *BRIGHT_MAGENTA = "\u001b[95m";
    static constexpr const char *BRIGHT_CYAN = "\u001b[96m";
    static constexpr const char *BRIGHT_WHITE = "\u001b[97m";
    static constexpr const char *BACKGROUND_BLACK = "\u001b[40m";
    static constexpr const char *BACKGROUND_RED = "\u001b[41m";
    static constexpr const char *BACKGROUND_GREEN = "\u001b[42m";
    static constexpr const char *BACKGROUND_YELLOW = "\u001b[43m";
    static constexpr const char *BACKGROUND_BLUE = "\u001b[44m";
    static constexpr const char *BACKGROUND_MAGENTA = "\u001b[45m";
    static constexpr const char *BACKGROUND_CYAN = "\u001b[46m";
    static constexpr const char *BACKGROUND_WHITE = "\u001b[47m";
    static constexpr const char *BACKGROUND_BRIGHT_BLACK = "\u001b[100m";
    static constexpr const char *BACKGROUND_BRIGHT_RED = "\u001b[101m";
    static constexpr const char *BACKGROUND_BRIGHT_GREEN = "\u001b[102m";
    static constexpr const char *BACKGROUND_BRIGHT_YELLOW = "\u001b[103m";
    static constexpr const char *BACKGROUND_BRIGHT_BLUE = "\u001b[104m";
    static constexpr const char *BACKGROUND_BRIGHT_MAGENTA = "\u001b[105m";
    static constexpr const char *BACKGROUND_BRIGHT_CYAN = "\u001b[106m";
    static constexpr const char *BACKGROUND_BRIGHT_WHITE = "\u001b[107m";

    static const char ESCAPE_END = 'm';

};

}

#endif
