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

#ifndef HHUOS_LIB_LUNAR_THEME_H
#define HHUOS_LIB_LUNAR_THEME_H

#include "util/graphic/Colors.h"
#include "lunar/Widget.h"

namespace Lunar {

/// Base class for all themes defining the visual appearance of widgets.
/// A theme consists of different styles for each widget type, each style defining colors for different widget states.
/// The global current theme can be accessed via `Theme::CURRENT_THEME` and changed via `Theme::setTheme()`.
/// Each widget uses the current theme to determine its colors when being drawn.
class Theme {

public:
    /// Destroy the theme instance.
    virtual ~Theme() = default;

    /// A style defines the colors that should be used for a specific widget state.
    /// This includes colors for the widget background, border, accent elements, text and text background.
    struct Style {
        /// The color of the widget background.
        Util::Graphic::Color widgetColor;
        /// The color of the widget border.
        Util::Graphic::Color borderColor;
        /// The accent color used for highlights or other accent elements.
        Util::Graphic::Color accentColor;
        /// The color of the text displayed on the widget.
        Util::Graphic::Color textColor;
        /// The background color of the text displayed on the widget.
        Util::Graphic::Color textBackgroundColor;
    };

    /// A widget style defines the styles for the different states of a widget (normal, hovered, pressed).
    struct WidgetStyle {
        /// Style for the normal state of the widget (i.e., not hovered or pressed).
        Style normalStyle;
        /// Style for the hovered state of the widget (i.e., mouse is over the widget).
        Style hoveredStyle;
        /// Style for the pressed state of the widget (i.e., mouse button is pressed on the widget).
        Style pressedStyle;

        /// Get the appropriate style for the given widget based on its current state.
        const Style& getStyle(const Widget &widget) const {
            return widget.isPressed() ? pressedStyle : widget.isHovered() ? hoveredStyle : normalStyle;
        }
    };

    /// Set the current global theme to the given theme instance.
    /// The previous theme instance is deleted.
    static void setTheme(const Theme *theme) {
        delete &CURRENT_THEME;
        CURRENT_THEME = *theme;
    }

    /// Get the style for container widgets.
    virtual const WidgetStyle& container() const = 0;

    /// Get the style for label widgets.
    virtual const WidgetStyle& label() const = 0;

    /// Get the style for button widgets.
    virtual const WidgetStyle& button() const = 0;

    /// Get the style for checkbox widgets.
    virtual const WidgetStyle& checkBox() const = 0;

    /// Get the style for radio button widgets.
    virtual const WidgetStyle& radioButton() const = 0;

    /// Get the style for input field widgets.
    virtual const WidgetStyle& inputField() const = 0;

    /// The current global theme instance.
    static Theme &CURRENT_THEME;
};

/// A theme based on the Heinrich Heine University Düsseldorf (HHU) corporate design.
class HhuTheme final : public Theme {

public:
    /// Get the style for container widgets.
    const WidgetStyle& container() const override {
        return containerStyle;
    }

    /// Get the style for label widgets.
    const WidgetStyle& label() const override {
        return labelStyle;
    }

    /// Get the style for button widgets.
    const WidgetStyle& button() const override {
        return buttonStyle;
    }

    /// Get the style for checkbox widgets.
    const WidgetStyle& checkBox() const override {
        return checkBoxStyle;
    }

    /// Get the style for radio button widgets.
    const WidgetStyle& radioButton() const override {
        return radioButtonStyle;
    }

    /// Get the style for input field widgets.
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
