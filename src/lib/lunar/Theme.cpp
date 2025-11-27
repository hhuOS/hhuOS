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

#include "Theme.h"

namespace Lunar {

Theme &Theme::CURRENT_THEME = *new HhuTheme();

const Theme::Style& Theme::WidgetStyle::getStyle(const Widget &widget) const {
    return widget.isPressed() ? pressedStyle : widget.isHovered() ? hoveredStyle : normalStyle;
}

void Theme::setTheme(const Theme *theme) {
    delete &CURRENT_THEME;
    CURRENT_THEME = *theme;
}

const Theme::WidgetStyle& HhuTheme::container() const {
    return containerStyle;
}

const Theme::WidgetStyle& HhuTheme::label() const {
    return labelStyle;
}

const Theme::WidgetStyle& HhuTheme::button() const {
    return buttonStyle;
}

const Theme::WidgetStyle& HhuTheme::checkBox() const {
    return checkBoxStyle;
}

const Theme::WidgetStyle& HhuTheme::radioButton() const {
    return radioButtonStyle;
}

const Theme::WidgetStyle& HhuTheme::inputField() const {
    return inputFieldStyle;
}

}
