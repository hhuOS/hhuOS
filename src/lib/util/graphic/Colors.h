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

#ifndef HHUOS_LIB_UTIL_GRAPHIC_COLORS_H
#define HHUOS_LIB_UTIL_GRAPHIC_COLORS_H

#include "Color.h"

namespace Util::Graphic::Colors {

/// An invisible color (alpha = 0)
/// This may be useful for transparent areas in images or graphics.
static Color INVISIBLE(0, 0, 0, 0);

// Standard CGA colors (bright version can be obtained by calling bright())

/// Black color (0, 0, 0).
static Color BLACK(0, 0, 0);
/// Red color (170, 0, 0).
static Color RED(170, 0, 0);
/// Green color (0, 170, 0).
static Color GREEN(0, 170, 0);
/// Yellow color (170, 170, 0).
static Color YELLOW(170, 170, 0);
/// Brown color (170, 85, 0).
static Color BROWN(170, 85, 0);
/// Blue color (0, 0, 170).
static Color BLUE(0, 0, 170);
/// Magenta color (170, 0, 170).
static Color MAGENTA(170, 0, 170);
/// Cyan color (0, 170, 170).
static Color CYAN(0, 170, 170);
/// White/Grey color (170, 170, 170).
static Color WHITE(170, 170, 170);

// Colors defined by Heinrich Heine University Düsseldorf

/// HHU blue color (0, 106, 179).
static Color HHU_BLUE(0, 106, 179);
/// HHU black color (0, 0, 0).
static Color HHU_BLACK(0, 0, 0);
/// HHU green color (140, 177, 16).
static Color HHU_GREEN(140, 177, 16);
/// HHU yellow color (238, 127, 0).
static Color HHU_YELLOW(238, 127, 0);
/// HHU red color (190, 10, 38).
static Color HHU_RED(190, 10, 38);
/// HHU turquoise color (87, 186, 177).
static Color HHU_TURQUOISE(87, 186, 177);
/// HHU dark blue color (0, 57, 100).
static Color HHU_DARK_BLUE(0, 57, 100);
/// HHU ice blue color (181, 203, 214).
static Color HHU_ICE_BLUE(181, 203, 214);

}

#endif