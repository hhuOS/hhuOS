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

        [[nodiscard]] const Style& getStyle(const Widget &widget) const;
    };

    static void setTheme(const Theme *theme);

    [[nodiscard]] virtual const WidgetStyle& container() const = 0;

    [[nodiscard]] virtual const WidgetStyle& label() const = 0;

    [[nodiscard]] virtual const WidgetStyle& button() const = 0;

    [[nodiscard]] virtual const WidgetStyle& checkBox() const = 0;

    [[nodiscard]] virtual const WidgetStyle& radioButton() const = 0;

    [[nodiscard]] virtual const WidgetStyle& inputField() const = 0;

    static Theme &CURRENT_THEME;

};

class HhuTheme final : public Theme {

public:

    [[nodiscard]] const WidgetStyle& container() const override;

    [[nodiscard]] const WidgetStyle& label() const override;

    [[nodiscard]] const WidgetStyle& button() const override;

    [[nodiscard]] const WidgetStyle& checkBox() const override;

    [[nodiscard]] const WidgetStyle& radioButton() const override;

    [[nodiscard]] const WidgetStyle& inputField() const override;

private:

    const WidgetStyle containerStyle = {
        .normalStyle = {
            .widgetColor = Util::Graphic::Colors::WHITE,
            .borderColor = Util::Graphic::Colors::WHITE,
            .accentColor = Util::Graphic::Colors::INVISIBLE,
            .textColor = Util::Graphic::Colors::INVISIBLE,
            .textBackgroundColor = Util::Graphic::Colors::INVISIBLE
        },
        .hoveredStyle = {
            .widgetColor = Util::Graphic::Colors::WHITE,
            .borderColor = Util::Graphic::Colors::WHITE,
            .accentColor = Util::Graphic::Colors::INVISIBLE,
            .textColor = Util::Graphic::Colors::INVISIBLE,
            .textBackgroundColor = Util::Graphic::Colors::INVISIBLE
        },
        .pressedStyle = {
            .widgetColor = Util::Graphic::Colors::WHITE,
            .borderColor = Util::Graphic::Colors::WHITE,
            .accentColor = Util::Graphic::Colors::INVISIBLE,
            .textColor = Util::Graphic::Colors::INVISIBLE,
            .textBackgroundColor = Util::Graphic::Colors::INVISIBLE
        }
    };

    WidgetStyle labelStyle = {
        .normalStyle = {
            .widgetColor = Util::Graphic::Colors::INVISIBLE,
            .borderColor = Util::Graphic::Colors::INVISIBLE,
            .accentColor = Util::Graphic::Colors::INVISIBLE,
            .textColor = Util::Graphic::Colors::HHU_BLACK,
            .textBackgroundColor = containerStyle.normalStyle.widgetColor
        },
        .hoveredStyle = {
            .widgetColor = Util::Graphic::Colors::INVISIBLE,
            .borderColor = Util::Graphic::Colors::INVISIBLE,
            .accentColor = Util::Graphic::Colors::INVISIBLE,
            .textColor = Util::Graphic::Colors::HHU_BLACK,
            .textBackgroundColor = containerStyle.hoveredStyle.widgetColor
        },
        .pressedStyle = {
            .widgetColor = Util::Graphic::Colors::INVISIBLE,
            .borderColor = Util::Graphic::Colors::INVISIBLE,
            .accentColor = Util::Graphic::Colors::INVISIBLE,
            .textColor = Util::Graphic::Colors::HHU_BLACK,
            .textBackgroundColor = containerStyle.pressedStyle.widgetColor
        }
    };

    WidgetStyle buttonStyle = {
        .normalStyle = {
            .widgetColor = Util::Graphic::Colors::HHU_BLUE.withSaturation(75),
            .borderColor = Util::Graphic::Colors::HHU_DARK_BLUE,
            .accentColor = Util::Graphic::Colors::INVISIBLE,
            .textColor = Util::Graphic::Colors::HHU_ICE_BLUE.withSaturation(35),
            .textBackgroundColor = Util::Graphic::Colors::HHU_BLUE.withSaturation(75)
        },
        .hoveredStyle = {
            .widgetColor = Util::Graphic::Colors::HHU_BLUE,
            .borderColor = Util::Graphic::Colors::HHU_DARK_BLUE,
            .accentColor = Util::Graphic::Colors::INVISIBLE,
            .textColor = Util::Graphic::Colors::HHU_ICE_BLUE.withSaturation(35),
            .textBackgroundColor = Util::Graphic::Colors::HHU_BLUE
        },
        .pressedStyle = {
            .widgetColor = Util::Graphic::Colors::HHU_DARK_BLUE,
            .borderColor = Util::Graphic::Colors::HHU_DARK_BLUE,
            .accentColor = Util::Graphic::Colors::INVISIBLE,
            .textColor = Util::Graphic::Colors::HHU_ICE_BLUE.withSaturation(35),
            .textBackgroundColor = Util::Graphic::Colors::HHU_DARK_BLUE
        }
    };

    WidgetStyle checkBoxStyle = {
        .normalStyle = {
            .widgetColor = Util::Graphic::Colors::HHU_ICE_BLUE.withSaturation(35),
            .borderColor = Util::Graphic::Colors::HHU_DARK_BLUE,
            .accentColor = Util::Graphic::Colors::HHU_BLUE,
            .textColor = labelStyle.normalStyle.textColor,
            .textBackgroundColor = labelStyle.normalStyle.textBackgroundColor
        },
        .hoveredStyle = {
            .widgetColor = Util::Graphic::Colors::HHU_ICE_BLUE.withSaturation(75),
            .borderColor = Util::Graphic::Colors::HHU_DARK_BLUE,
            .accentColor = Util::Graphic::Colors::HHU_BLUE,
            .textColor = labelStyle.hoveredStyle.textColor,
            .textBackgroundColor = labelStyle.hoveredStyle.textBackgroundColor
        },
        .pressedStyle = {
            .widgetColor = Util::Graphic::Colors::HHU_ICE_BLUE,
            .borderColor = Util::Graphic::Colors::HHU_DARK_BLUE,
            .accentColor = Util::Graphic::Colors::HHU_BLUE,
            .textColor = labelStyle.pressedStyle.textColor,
            .textBackgroundColor = labelStyle.pressedStyle.textBackgroundColor
        }
    };

    WidgetStyle radioButtonStyle = checkBoxStyle;

    WidgetStyle inputFieldStyle = {
        .normalStyle = {
            .widgetColor = checkBoxStyle.normalStyle.widgetColor,
            .borderColor = checkBoxStyle.normalStyle.borderColor,
            .accentColor = labelStyle.normalStyle.textColor,
            .textColor = labelStyle.normalStyle.textColor,
            .textBackgroundColor = checkBoxStyle.normalStyle.widgetColor
        },
        .hoveredStyle = {
            .widgetColor = checkBoxStyle.normalStyle.widgetColor,
            .borderColor = checkBoxStyle.normalStyle.borderColor,
            .accentColor = labelStyle.hoveredStyle.textColor,
            .textColor = labelStyle.hoveredStyle.textColor,
            .textBackgroundColor = checkBoxStyle.normalStyle.widgetColor
        },
        .pressedStyle = {
            .widgetColor = checkBoxStyle.normalStyle.widgetColor,
            .borderColor = checkBoxStyle.normalStyle.borderColor,
            .accentColor = labelStyle.pressedStyle.textColor,
            .textColor = labelStyle.pressedStyle.textColor,
            .textBackgroundColor = checkBoxStyle.normalStyle.widgetColor
        }
    };

};

}

#endif
