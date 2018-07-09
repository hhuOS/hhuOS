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

#include "Colors.h"

Color Colors::INVISIBLE = Color(0, 0, 0, 0);

// CGA colors
Color Colors::BLACK = Color(0, 0, 0);
Color Colors::BLUE = Color(0, 0, 170);
Color Colors::GREEN = Color(0, 170, 0);
Color Colors::CYAN = Color(0, 170, 170);
Color Colors::RED = Color(170, 0, 0);
Color Colors::MAGENTA = Color(170, 0, 170);
Color Colors::DARK_YELLOW = Color(170, 170, 0);
Color Colors::LIGHT_GRAY = Color(170, 170, 170);
Color Colors::GRAY = Color(85, 85, 85);
Color Colors::LIGHT_BLUE = Color(85, 85, 255);
Color Colors::LIGHT_GREEN = Color(85, 255, 85);
Color Colors::LIGHT_CYAN = Color(85, 255, 255);
Color Colors::LIGHT_RED = Color(255, 85, 85);
Color Colors::LIGHT_MAGENTA = Color(255, 85, 255);
Color Colors::YELLOW = Color(255, 255, 85);
Color Colors::WHITE = Color(255, 255, 255);

// HHU primary colors
Color Colors::HHU_BLUE = Color(0, 106, 179);
Color Colors::HHU_BLUE_70 = Color(54, 128, 179);
Color Colors::HHU_BLUE_50 = Color(90, 142, 179);
Color Colors::HHU_BLUE_30 = Color(125, 157, 179);
Color Colors::HHU_BLUE_10 = Color(161, 172, 179);

Color Colors::HHU_GRAY = Color(217, 218, 219);
Color Colors::HHU_LIGHT_GRAY = Color(236, 237, 237);

// HHU secondary colors
Color Colors::HHU_GREEN = Color(151, 191, 13);
Color Colors::HHU_GREEN_70 = Color(159, 191, 57);
Color Colors::HHU_GREEN_50 = Color(168, 191, 96);
Color Colors::HHU_GREEN_30 = Color(177, 191, 134);
Color Colors::HHU_GREEN_10 = Color(186, 191, 172);

Color Colors::HHU_RED = Color(190, 10, 38);
Color Colors::HHU_RED_70 = Color(190, 57, 78);
Color Colors::HHU_RED_50 = Color(190, 95, 110);
Color Colors::HHU_RED_30 = Color(190, 133, 142);
Color Colors::HHU_RED_10 = Color(190, 171, 174);

Color Colors::HHU_DARK_BLUE = Color(0, 56, 101);
Color Colors::HHU_DARK_BLUE_70 = Color(30, 70, 101);
Color Colors::HHU_DARK_BLUE_50 = Color(51, 79, 101);
Color Colors::HHU_DARK_BLUE_30 = Color(71, 88, 101);
Color Colors::HHU_DARK_BLUE_10 = Color(91, 97, 101);

Color Colors::HHU_YELLOW = Color(242, 148, 0);
Color Colors::HHU_YELLOW_70 = Color(242, 176, 73);
Color Colors::HHU_YELLOW_50 = Color(242, 195, 121);
Color Colors::HHU_YELLOW_30 = Color(242, 214, 169);
Color Colors::HHU_YELLOW_10 = Color(242, 233, 218);

Color Colors::HHU_TURQUOISE = Color(50, 184, 201);
Color Colors::HHU_TURQUOISE_70 = Color(60, 185, 201);
Color Colors::HHU_TURQUOISE_50 = Color(101, 190, 201);
Color Colors::HHU_TURQUOISE_30 = Color(141, 194, 201);
Color Colors::HHU_TURQUOISE_10 = Color(181, 199, 201);

// Terminal colors

Color Colors::TERM_BLACK = Color(0, 0, 0);
Color Colors::TERM_RED = Color(205, 0, 0);
Color Colors::TERM_GREEN = Color(0, 205, 0);
Color Colors::TERM_YELLOW = Color(205, 205, 0);
Color Colors::TERM_BLUE = Color(0, 0, 238);
Color Colors::TERM_MAGENTA = Color(205, 0, 205);
Color Colors::TERM_CYAN = Color(0, 205, 205);
Color Colors::TERM_WHITE = Color(229, 229, 229);

Color Colors::TERM_BRIGHT_BLACK = Color(127, 127, 127);
Color Colors::TERM_BRIGHT_RED = Color(255, 0, 0);
Color Colors::TERM_BRIGHT_GREEN = Color(0, 255, 0);
Color Colors::TERM_BRIGHT_YELLOW = Color(255, 255, 0);
Color Colors::TERM_BRIGHT_BLUE = Color(92, 92, 255);
Color Colors::TERM_BRIGHT_MAGENTA = Color(255, 0, 255);
Color Colors::TERM_BRIGHT_CYAN = Color(0, 255, 255);
Color Colors::TERM_BRIGHT_WHITE = Color(255, 255, 255);