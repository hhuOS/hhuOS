/*
 * Copyright (C) 2018 Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schoettner
 * Heinrich-Heine University
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

class Ansi {

public:

    static constexpr const char *RESET = "\u001b[0m";

    static constexpr const char *BLACK = "\u001b[30m";

    static constexpr const char *RED = "\u001b[31m";

    static constexpr const char *GREEN = "\u001b[32m";

    static constexpr const char *YELLOW = "\u001b[33m";

    static constexpr const char *BLUE = "\u001b[34m";

    static constexpr const char *MAGENTA = "\u001b[35m";

    static constexpr const char *CYAN = "\u001b[36m";

    static constexpr const char *WHITE = "\u001b[37m";

    static constexpr const char *BRIGHT_BLACK = "\u001b[30;1m";

    static constexpr const char *BRIGHT_RED = "\u001b[31;1m";

    static constexpr const char *BRIGHT_GREEN = "\u001b[32;1m";

    static constexpr const char *BRIGHT_YELLOW = "\u001b[33;1m";

    static constexpr const char *BRIGHT_BLUE = "\u001b[34;1m";

    static constexpr const char *BRIGHT_MAGENTA = "\u001b[35;1m";

    static constexpr const char *BRIGHT_CYAN = "\u001b[36;1m";

    static constexpr const char *BRIGHT_WHITE = "\u001b[37;1m";

    static constexpr const char *BACKGROUND_BLACK = "\u001b[40m";

    static constexpr const char *BACKGROUND_RED = "\u001b[41m";

    static constexpr const char *BACKGROUND_GREEN = "\u001b[42m";

    static constexpr const char *BACKGROUND_YELLOW = "\u001b[43m";

    static constexpr const char *BACKGROUND_BLUE = "\u001b[44m";

    static constexpr const char *BACKGROUND_MAGENTA = "\u001b[45m";

    static constexpr const char *BACKGROUND_CYAN = "\u001b[46m";

    static constexpr const char *BACKGROUND_WHITE = "\u001b[47m";

    static constexpr const char *BACKGROUND_BRIGHT_BLACK = "\u001b[40;1m";

    static constexpr const char *BACKGROUND_BRIGHT_RED = "\u001b[41;1m";

    static constexpr const char *BACKGROUND_BRIGHT_GREEN = "\u001b[42;1m";

    static constexpr const char *BACKGROUND_BRIGHT_YELLOW = "\u001b[43;1m";

    static constexpr const char *BACKGROUND_BRIGHT_BLUE = "\u001b[44;1m";

    static constexpr const char *BACKGROUND_BRIGHT_MAGENTA = "\u001b[45;1m";

    static constexpr const char *BACKGROUND_BRIGHT_CYAN = "\u001b[46;1m";

    static constexpr const char *BACKGROUND_BRIGHT_WHITE = "\u001b[47;1m";

    static const char ESCAPE_END = 'm';

};

#endif
