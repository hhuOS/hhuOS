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
#include "util/graphic/widget/Widget.h"

namespace Util::Graphic {

class Theme {

public:

    virtual ~Theme() = default;

    struct Style {
        Color widgetColor;
        Color borderColor;
        Color accentColor;
        Color textColor;
        Color textBackgroundColor;
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
            .widgetColor = Colors::WHITE,
            .borderColor = Colors::WHITE,
            .accentColor = Colors::INVISIBLE,
            .textColor = Colors::INVISIBLE,
            .textBackgroundColor = Colors::INVISIBLE
        },
        .hoveredStyle = {
            .widgetColor = Colors::WHITE,
            .borderColor = Colors::WHITE,
            .accentColor = Colors::INVISIBLE,
            .textColor = Colors::INVISIBLE,
            .textBackgroundColor = Colors::INVISIBLE
        },
        .pressedStyle = {
            .widgetColor = Colors::WHITE,
            .borderColor = Colors::WHITE,
            .accentColor = Colors::INVISIBLE,
            .textColor = Colors::INVISIBLE,
            .textBackgroundColor = Colors::INVISIBLE
        }
    };

    WidgetStyle labelStyle = {
        .normalStyle = {
            .widgetColor = Colors::INVISIBLE,
            .borderColor = Colors::INVISIBLE,
            .accentColor = Colors::INVISIBLE,
            .textColor = Colors::HHU_BLACK,
            .textBackgroundColor = containerStyle.normalStyle.widgetColor
        },
        .hoveredStyle = {
            .widgetColor = Colors::INVISIBLE,
            .borderColor = Colors::INVISIBLE,
            .accentColor = Colors::INVISIBLE,
            .textColor = Colors::HHU_BLACK,
            .textBackgroundColor = containerStyle.hoveredStyle.widgetColor
        },
        .pressedStyle = {
            .widgetColor = Colors::INVISIBLE,
            .borderColor = Colors::INVISIBLE,
            .accentColor = Colors::INVISIBLE,
            .textColor = Colors::HHU_BLACK,
            .textBackgroundColor = containerStyle.pressedStyle.widgetColor
        }
    };

    WidgetStyle buttonStyle = {
        .normalStyle = {
            .widgetColor = Colors::HHU_BLUE.withSaturation(75),
            .borderColor = Colors::HHU_DARK_BLUE,
            .accentColor = Colors::INVISIBLE,
            .textColor = Colors::HHU_ICE_BLUE.withSaturation(35),
            .textBackgroundColor = Colors::HHU_BLUE.withSaturation(75)
        },
        .hoveredStyle = {
            .widgetColor = Colors::HHU_BLUE,
            .borderColor = Colors::HHU_DARK_BLUE,
            .accentColor = Colors::INVISIBLE,
            .textColor = Colors::HHU_ICE_BLUE.withSaturation(35),
            .textBackgroundColor = Colors::HHU_BLUE
        },
        .pressedStyle = {
            .widgetColor = Colors::HHU_DARK_BLUE,
            .borderColor = Colors::HHU_DARK_BLUE,
            .accentColor = Colors::INVISIBLE,
            .textColor = Colors::HHU_ICE_BLUE.withSaturation(35),
            .textBackgroundColor = Colors::HHU_DARK_BLUE
        }
    };

    WidgetStyle checkBoxStyle = {
        .normalStyle = {
            .widgetColor = Colors::HHU_ICE_BLUE.withSaturation(35),
            .borderColor = Colors::HHU_DARK_BLUE,
            .accentColor = Colors::HHU_BLUE,
            .textColor = labelStyle.normalStyle.textColor,
            .textBackgroundColor = labelStyle.normalStyle.textBackgroundColor
        },
        .hoveredStyle = {
            .widgetColor = Colors::HHU_ICE_BLUE.withSaturation(75),
            .borderColor = Colors::HHU_DARK_BLUE,
            .accentColor = Colors::HHU_BLUE,
            .textColor = labelStyle.hoveredStyle.textColor,
            .textBackgroundColor = labelStyle.hoveredStyle.textBackgroundColor
        },
        .pressedStyle = {
            .widgetColor = Colors::HHU_ICE_BLUE,
            .borderColor = Colors::HHU_DARK_BLUE,
            .accentColor = Colors::HHU_BLUE,
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
