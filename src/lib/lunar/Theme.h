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

#ifndef HHUOS_LIB_UTIL_GRAPHIC_WIDGET_THEME_H
#define HHUOS_LIB_UTIL_GRAPHIC_WIDGET_THEME_H

#include "util/graphic/Colors.h"
#include "lunar/Widget.h"

namespace Lunar {

class Theme {

public:

    virtual ~Theme() = default;

    struct Style {
        Util::Graphic::Color widgetColor;
        Util::Graphic::Color borderColor;
        Util::Graphic::Color accentColor;
        Util::Graphic::Color textColor;
        Util::Graphic::Color textBackgroundColor;
    };

    struct WidgetStyle {
        Style normalStyle;
        Style hoveredStyle;
        Style pressedStyle;

        const Style& getStyle(const Widget &widget) const {
            return widget.isPressed() ? pressedStyle : widget.isHovered() ? hoveredStyle : normalStyle;
        }
    };

    static void setTheme(const Theme *theme) {
        delete &CURRENT_THEME;
        CURRENT_THEME = *theme;
    }

    virtual const WidgetStyle& container() const = 0;

    virtual const WidgetStyle& label() const = 0;

    virtual const WidgetStyle& button() const = 0;

    virtual const WidgetStyle& checkBox() const = 0;

    virtual const WidgetStyle& radioButton() const = 0;

    virtual const WidgetStyle& inputField() const = 0;

    static Theme &CURRENT_THEME;
};

class HhuTheme final : public Theme {

public:

    const WidgetStyle& container() const override {
        return containerStyle;
    }

    const WidgetStyle& label() const override {
        return labelStyle;
    }

    const WidgetStyle& button() const override {
        return buttonStyle;
    }

    const WidgetStyle& checkBox() const override {
        return checkBoxStyle;
    }

    const WidgetStyle& radioButton() const override {
        return radioButtonStyle;
    }

    const WidgetStyle& inputField() const override {
        return inputFieldStyle;
    }

private:

    const WidgetStyle containerStyle = {
        {
            Util::Graphic::Colors::WHITE,
            Util::Graphic::Colors::WHITE,
            Util::Graphic::Colors::INVISIBLE,
            Util::Graphic::Colors::INVISIBLE,
            Util::Graphic::Colors::INVISIBLE
        },
        {
            Util::Graphic::Colors::WHITE,
            Util::Graphic::Colors::WHITE,
            Util::Graphic::Colors::INVISIBLE,
            Util::Graphic::Colors::INVISIBLE,
            Util::Graphic::Colors::INVISIBLE
        },
        {
            Util::Graphic::Colors::WHITE,
            Util::Graphic::Colors::WHITE,
            Util::Graphic::Colors::INVISIBLE,
            Util::Graphic::Colors::INVISIBLE,
            Util::Graphic::Colors::INVISIBLE
        }
    };

    WidgetStyle labelStyle = {
        {
            Util::Graphic::Colors::INVISIBLE,
            Util::Graphic::Colors::INVISIBLE,
            Util::Graphic::Colors::INVISIBLE,
            Util::Graphic::Colors::HHU_BLACK,
            containerStyle.normalStyle.widgetColor
        },
        {
            Util::Graphic::Colors::INVISIBLE,
            Util::Graphic::Colors::INVISIBLE,
            Util::Graphic::Colors::INVISIBLE,
            Util::Graphic::Colors::HHU_BLACK,
            containerStyle.hoveredStyle.widgetColor
        },
        {
            Util::Graphic::Colors::INVISIBLE,
            Util::Graphic::Colors::INVISIBLE,
            Util::Graphic::Colors::INVISIBLE,
            Util::Graphic::Colors::HHU_BLACK,
            containerStyle.pressedStyle.widgetColor
        }
    };

    WidgetStyle buttonStyle = {
        {
            Util::Graphic::Colors::HHU_BLUE.withSaturation(75),
            Util::Graphic::Colors::HHU_DARK_BLUE,
            Util::Graphic::Colors::INVISIBLE,
            Util::Graphic::Colors::HHU_ICE_BLUE.withSaturation(35),
            Util::Graphic::Colors::HHU_BLUE.withSaturation(75)
        },
        {
            Util::Graphic::Colors::HHU_BLUE,
            Util::Graphic::Colors::HHU_DARK_BLUE,
            Util::Graphic::Colors::INVISIBLE,
            Util::Graphic::Colors::HHU_ICE_BLUE.withSaturation(35),
            Util::Graphic::Colors::HHU_BLUE
        },
        {
            Util::Graphic::Colors::HHU_DARK_BLUE,
            Util::Graphic::Colors::HHU_DARK_BLUE,
            Util::Graphic::Colors::INVISIBLE,
            Util::Graphic::Colors::HHU_ICE_BLUE.withSaturation(35),
            Util::Graphic::Colors::HHU_DARK_BLUE
        }
    };

    WidgetStyle checkBoxStyle = {
        {
            Util::Graphic::Colors::HHU_ICE_BLUE.withSaturation(35),
            Util::Graphic::Colors::HHU_DARK_BLUE,
            Util::Graphic::Colors::HHU_BLUE,
            labelStyle.normalStyle.textColor,
            labelStyle.normalStyle.textBackgroundColor
        },
        {
            Util::Graphic::Colors::HHU_ICE_BLUE.withSaturation(75),
            Util::Graphic::Colors::HHU_DARK_BLUE,
            Util::Graphic::Colors::HHU_BLUE,
            labelStyle.hoveredStyle.textColor,
            labelStyle.hoveredStyle.textBackgroundColor
        },
        {
            Util::Graphic::Colors::HHU_ICE_BLUE,
            Util::Graphic::Colors::HHU_DARK_BLUE,
            Util::Graphic::Colors::HHU_BLUE,
            labelStyle.pressedStyle.textColor,
            labelStyle.pressedStyle.textBackgroundColor
        }
    };

    WidgetStyle radioButtonStyle = checkBoxStyle;

    WidgetStyle inputFieldStyle = {
        {
            checkBoxStyle.normalStyle.widgetColor,
            checkBoxStyle.normalStyle.borderColor,
            labelStyle.normalStyle.textColor,
            labelStyle.normalStyle.textColor,
            checkBoxStyle.normalStyle.widgetColor
        },
        {
            checkBoxStyle.normalStyle.widgetColor,
            checkBoxStyle.normalStyle.borderColor,
            labelStyle.hoveredStyle.textColor,
            labelStyle.hoveredStyle.textColor,
            checkBoxStyle.normalStyle.widgetColor
        },
        {
            checkBoxStyle.normalStyle.widgetColor,
            checkBoxStyle.normalStyle.borderColor,
            labelStyle.pressedStyle.textColor,
            labelStyle.pressedStyle.textColor,
            checkBoxStyle.normalStyle.widgetColor
        }
    };
};

}

#endif
