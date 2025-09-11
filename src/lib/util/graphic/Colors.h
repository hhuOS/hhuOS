/*
 * Copyright (C) 2017-2025 Heinrich Heine University Düsseldorf,
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
static Color BROWN = Color(170, 85, 0);
static Color BLUE = Color(0, 0, 170);
static Color MAGENTA = Color(170, 0, 170);
static Color CYAN = Color(0, 170, 170);
static Color WHITE = Color(170, 170, 170);

static Color HHU_BLUE = Color(0, 106, 179);
static Color HHU_BLACK = Color(0, 0, 0);
static Color HHU_GREEN = Color(140, 177, 16);
static Color HHU_YELLOW = Color(238, 127, 0);
static Color HHU_RED = Color(190, 10, 38);
static Color HHU_TURQUOISE = Color(87, 186, 177);
static Color HHU_DARK_BLUE = Color(0, 57, 100);
static Color HHU_ICE_BLUE = Color(181, 203, 214);

}

#endif