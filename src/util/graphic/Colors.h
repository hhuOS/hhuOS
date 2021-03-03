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

#ifndef __Colors_include__
#define __Colors_include__

#include "Color.h"

namespace Util::Graphic::Colors {

static Color INVISIBLE = Color(0, 0, 0, 0);

// CGA colors
static Color BLACK = Color(0, 0, 0);
static Color BLUE = Color(0, 0, 170);
static Color GREEN = Color(0, 170, 0);
static Color CYAN = Color(0, 170, 170);
static Color RED = Color(170, 0, 0);
static Color MAGENTA = Color(170, 0, 170);
static Color DARK_YELLOW = Color(170, 170, 0);
static Color LIGHT_GRAY = Color(170, 170, 170);
static Color GRAY = Color(85, 85, 85);
static Color LIGHT_BLUE = Color(85, 85, 255);
static Color LIGHT_GREEN = Color(85, 255, 85);
static Color LIGHT_CYAN = Color(85, 255, 255);
static Color LIGHT_RED = Color(255, 85, 85);
static Color LIGHT_MAGENTA = Color(255, 85, 255);
static Color YELLOW = Color(255, 255, 85);
static Color WHITE = Color(255, 255, 255);

// HHU primary colors
static Color HHU_BLUE = Color(0, 106, 179);
static Color HHU_BLUE_70 = Color(54, 128, 179);
static Color HHU_BLUE_50 = Color(90, 142, 179);
static Color HHU_BLUE_30 = Color(125, 157, 179);
static Color HHU_BLUE_10 = Color(161, 172, 179);

static Color HHU_GRAY = Color(217, 218, 219);
static Color HHU_LIGHT_GRAY = Color(236, 237, 237);

// HHU secondary colors
static Color HHU_GREEN = Color(151, 191, 13);
static Color HHU_GREEN_70 = Color(159, 191, 57);
static Color HHU_GREEN_50 = Color(168, 191, 96);
static Color HHU_GREEN_30 = Color(177, 191, 134);
static Color HHU_GREEN_10 = Color(186, 191, 172);

static Color HHU_RED = Color(190, 10, 38);
static Color HHU_RED_70 = Color(190, 57, 78);
static Color HHU_RED_50 = Color(190, 95, 110);
static Color HHU_RED_30 = Color(190, 133, 142);
static Color HHU_RED_10 = Color(190, 171, 174);

static Color HHU_DARK_BLUE = Color(0, 56, 101);
static Color HHU_DARK_BLUE_70 = Color(30, 70, 101);
static Color HHU_DARK_BLUE_50 = Color(51, 79, 101);
static Color HHU_DARK_BLUE_30 = Color(71, 88, 101);
static Color HHU_DARK_BLUE_10 = Color(91, 97, 101);

static Color HHU_YELLOW = Color(242, 148, 0);
static Color HHU_YELLOW_70 = Color(242, 176, 73);
static Color HHU_YELLOW_50 = Color(242, 195, 121);
static Color HHU_YELLOW_30 = Color(242, 214, 169);
static Color HHU_YELLOW_10 = Color(242, 233, 218);

static Color HHU_TURQUOISE = Color(50, 184, 201);
static Color HHU_TURQUOISE_70 = Color(60, 185, 201);
static Color HHU_TURQUOISE_50 = Color(101, 190, 201);
static Color HHU_TURQUOISE_30 = Color(141, 194, 201);
static Color HHU_TURQUOISE_10 = Color(181, 199, 201);

// LinearFrameBufferTerminal colors

static Color TERM_BLACK = Color(0, 0, 0);
static Color TERM_RED = Color(205, 0, 0);
static Color TERM_GREEN = Color(0, 205, 0);
static Color TERM_YELLOW = Color(205, 205, 0);
static Color TERM_BLUE = Color(0, 0, 238);
static Color TERM_MAGENTA = Color(205, 0, 205);
static Color TERM_CYAN = Color(0, 205, 205);
static Color TERM_WHITE = Color(229, 229, 229);

static Color TERM_BRIGHT_BLACK = Color(127, 127, 127);
static Color TERM_BRIGHT_RED = Color(255, 0, 0);
static Color TERM_BRIGHT_GREEN = Color(0, 255, 0);
static Color TERM_BRIGHT_YELLOW = Color(255, 255, 0);
static Color TERM_BRIGHT_BLUE = Color(92, 92, 255);
static Color TERM_BRIGHT_MAGENTA = Color(255, 0, 255);
static Color TERM_BRIGHT_CYAN = Color(0, 255, 255);
static Color TERM_BRIGHT_WHITE = Color(255, 255, 255);

}

#endif