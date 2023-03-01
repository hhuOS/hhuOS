/*
 * Copyright (C) 2018-2023 Heinrich-Heine-Universitaet Duesseldorf,
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

// ANSI colors
static Color BLACK = Color(0, 0, 0);
static Color RED = Color(170, 0, 0);
static Color GREEN = Color(0, 170, 0);
static Color YELLOW = Color(170, 170, 0);
static Color BLUE = Color(0, 0, 170);
static Color MAGENTA = Color(170, 0, 170);
static Color CYAN = Color(0, 170, 170);
static Color WHITE = Color(170, 170, 170);

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

}

#endif