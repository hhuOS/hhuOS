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
 *
 * The widget and layout system is based on a bachelor's thesis, written by Michael Zuchniewski.
 * The original source code can be found here: https://git.hhu.de/bsinfo/thesis/ba-mizuc100
 */

#include "Style.h"

namespace Util::Graphic::DefaultTheme {

Style container() {
    return Style {
        .textColor = Colors::INVISIBLE,
        .backgroundColor = Colors::HHU_GRAY,
        .textColorHighlighted = Colors::INVISIBLE,
        .backgroundColorHighlighted = Colors::HHU_GRAY,
        .borderColor = Colors::HHU_GRAY.dim(),
        .accentColor = Colors::HHU_GRAY.dim(),
        .paddingX = 8,
        .paddingY = 8,
        .gapX = 4
    };
}

Style label() {
    return Style {
        .textColor = Colors::HHU_GRAY.dim().dim(),
        .backgroundColor = Colors::INVISIBLE,
        .textColorHighlighted = Colors::HHU_GRAY.dim().dim(),
        .backgroundColorHighlighted = Colors::INVISIBLE,
        .borderColor = Colors::INVISIBLE,
        .accentColor = Colors::INVISIBLE,
        .paddingX = 2,
        .paddingY = 0,
        .gapX = 4
    };
}

Style button() {
    return Style {
        .textColor = Colors::HHU_LIGHT_GRAY.dim(),
        .backgroundColor = Colors::HHU_BLUE,
        .textColorHighlighted = Colors::HHU_GRAY.dim().dim(),
        .backgroundColorHighlighted = Colors::HHU_DARK_BLUE,
        .borderColor = Colors::HHU_DARK_BLUE,
        .accentColor = Colors::HHU_BLUE,
        .paddingX = 8,
        .paddingY = 6,
        .gapX = 4
    };
}

Style checkBox() {
    return Style {
        .textColor = Colors::HHU_GRAY.dim().dim(),
        .backgroundColor = Colors::HHU_LIGHT_GRAY,
        .textColorHighlighted = Colors::HHU_GRAY.dim().dim(),
        .backgroundColorHighlighted = Colors::HHU_LIGHT_GRAY.dim(),
        .borderColor = Colors::HHU_LIGHT_GRAY.dim(),
        .accentColor = Colors::HHU_BLUE,
        .paddingX = 4,
        .paddingY = 2,
        .gapX = 6
    };
}

Style radioButton() {
    return Style {
        .textColor = Colors::HHU_GRAY.dim().dim(),
        .backgroundColor = Colors::HHU_LIGHT_GRAY,
        .textColorHighlighted = Colors::HHU_GRAY.dim().dim(),
        .backgroundColorHighlighted = Colors::HHU_LIGHT_GRAY.dim(),
        .borderColor = Colors::HHU_LIGHT_GRAY.dim(),
        .accentColor = Colors::HHU_BLUE,
        .paddingX = 4,
        .paddingY = 2,
        .gapX = 6
    };
}
}
