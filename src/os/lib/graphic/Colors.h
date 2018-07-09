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

#ifndef __Colors_include__
#define __Colors_include__

#include "Color.h"

namespace Colors {
    extern Color INVISIBLE;

    // CGA colors
    extern Color BLACK;
    extern Color BLUE;
    extern Color GREEN;
    extern Color CYAN;
    extern Color RED;
    extern Color MAGENTA;
    extern Color DARK_YELLOW;
    extern Color LIGHT_GRAY;
    extern Color GRAY;
    extern Color LIGHT_BLUE;
    extern Color LIGHT_GREEN;
    extern Color LIGHT_CYAN;
    extern Color LIGHT_RED;
    extern Color LIGHT_MAGENTA;
    extern Color YELLOW;
    extern Color WHITE;

    // HHU primary colors
    extern Color HHU_BLUE;
    extern Color HHU_BLUE_70;
    extern Color HHU_BLUE_50;
    extern Color HHU_BLUE_30;
    extern Color HHU_BLUE_10;

    extern Color HHU_GRAY;
    extern Color HHU_LIGHT_GRAY;

    // HHU secondary colors
    extern Color HHU_GREEN;
    extern Color HHU_GREEN_70;
    extern Color HHU_GREEN_50;
    extern Color HHU_GREEN_30;
    extern Color HHU_GREEN_10;

    extern Color HHU_RED;
    extern Color HHU_RED_70;
    extern Color HHU_RED_50;
    extern Color HHU_RED_30;
    extern Color HHU_RED_10;

    extern Color HHU_DARK_BLUE;
    extern Color HHU_DARK_BLUE_70;
    extern Color HHU_DARK_BLUE_50;
    extern Color HHU_DARK_BLUE_30;
    extern Color HHU_DARK_BLUE_10;

    extern Color HHU_YELLOW;
    extern Color HHU_YELLOW_70;
    extern Color HHU_YELLOW_50;
    extern Color HHU_YELLOW_30;
    extern Color HHU_YELLOW_10;

    extern Color HHU_TURQUOISE;
    extern Color HHU_TURQUOISE_70;
    extern Color HHU_TURQUOISE_50;
    extern Color HHU_TURQUOISE_30;
    extern Color HHU_TURQUOISE_10;

    // Terminal colors
    extern Color TERM_BLACK;
    extern Color TERM_RED;
    extern Color TERM_GREEN;
    extern Color TERM_YELLOW;
    extern Color TERM_BLUE;
    extern Color TERM_MAGENTA;
    extern Color TERM_CYAN;
    extern Color TERM_WHITE;

    extern Color TERM_BRIGHT_BLACK;
    extern Color TERM_BRIGHT_RED;
    extern Color TERM_BRIGHT_GREEN;
    extern Color TERM_BRIGHT_YELLOW;
    extern Color TERM_BRIGHT_BLUE;
    extern Color TERM_BRIGHT_MAGENTA;
    extern Color TERM_BRIGHT_CYAN;
    extern Color TERM_BRIGHT_WHITE;

}

#endif