/*
 * Copyright (C) 2017-2026 Heinrich Heine University Düsseldorf,
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

#ifndef HHUOS_TITLEBAR_H
#define HHUOS_TITLEBAR_H

#include "WindowMouseEvent.h"
#include "lunar/Container.h"
#include "lunar/Label.h"
#include "lunar/Theme.h"

class TitleBar {

public:

    explicit TitleBar(const Util::String &title = "");

    void setTitle(const Util::String &title) const {
        titleLabel->setText(title);
    }

    const Util::String& getTitle() const {
        return titleLabel->getText();
    }

    size_t getHeight() const {
        return height;
    }

    void draw(const Util::Graphic::LinearFrameBuffer &lfb, int32_t posX, int32_t posY, uint16_t width, bool focused = false);

    void onMouseHover(const MouseEvent &event);

    void onMouseExit();

    void onMouseClick(const MouseEvent &event);

    void onMouseRelease(const MouseEvent &event);

    bool needsRedraw() const;

private:

    Lunar::Container rootContainer;
    Lunar::Container *titleContainer = new Lunar::Container();
    Lunar::Container *buttonContainer = new Lunar::Container();
    Lunar::Label *titleLabel = nullptr;

    size_t height = 0;

    Lunar::Widget *lastHoveredChild = nullptr;
    Lunar::Widget *lastPressedChild = nullptr;
};

static const Lunar::Theme::WidgetStyle FOCUSED_TITLE_BAR_STYLE = {
    {
        Util::Graphic::Colors::HHU_BLUE,
        Util::Graphic::Colors::HHU_BLUE,
        Util::Graphic::Colors::INVISIBLE,
        Util::Graphic::Colors::INVISIBLE,
        Util::Graphic::Colors::INVISIBLE
    },
    {
        Util::Graphic::Colors::HHU_BLUE,
        Util::Graphic::Colors::HHU_BLUE,
        Util::Graphic::Colors::INVISIBLE,
        Util::Graphic::Colors::INVISIBLE,
        Util::Graphic::Colors::INVISIBLE
    },
    {
        Util::Graphic::Colors::HHU_BLUE,
        Util::Graphic::Colors::HHU_BLUE,
        Util::Graphic::Colors::INVISIBLE,
        Util::Graphic::Colors::INVISIBLE,
        Util::Graphic::Colors::INVISIBLE
    }
};

static const Lunar::Theme::WidgetStyle CLOSE_BUTTON_STYLE = {
    {
        Util::Graphic::Colors::HHU_RED.withSaturation(50),
        Util::Graphic::Colors::HHU_RED,
        Util::Graphic::Colors::INVISIBLE,
        Util::Graphic::Colors::BLACK,
        Util::Graphic::Colors::HHU_RED.withSaturation(50)
    },
    {
        Util::Graphic::Colors::HHU_RED.withSaturation(75),
        Util::Graphic::Colors::HHU_RED,
        Util::Graphic::Colors::INVISIBLE,
        Util::Graphic::Colors::BLACK,
        Util::Graphic::Colors::HHU_RED.withSaturation(75)
    },
    {
        Util::Graphic::Colors::HHU_RED,
        Util::Graphic::Colors::HHU_RED,
        Util::Graphic::Colors::INVISIBLE,
        Util::Graphic::Colors::BLACK,
        Util::Graphic::Colors::HHU_RED
    }
};

class TitleBarTheme final : public Lunar::Theme {

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
            Util::Graphic::Colors::HHU_ICE_BLUE,
            Util::Graphic::Colors::HHU_ICE_BLUE,
            Util::Graphic::Colors::INVISIBLE,
            Util::Graphic::Colors::INVISIBLE,
            Util::Graphic::Colors::INVISIBLE
        },
        {
            Util::Graphic::Colors::HHU_ICE_BLUE,
            Util::Graphic::Colors::HHU_ICE_BLUE,
            Util::Graphic::Colors::INVISIBLE,
            Util::Graphic::Colors::INVISIBLE,
            Util::Graphic::Colors::INVISIBLE
        },
        {
            Util::Graphic::Colors::HHU_ICE_BLUE,
            Util::Graphic::Colors::HHU_ICE_BLUE,
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
            Util::Graphic::Colors::INVISIBLE
        },
        {
            Util::Graphic::Colors::INVISIBLE,
            Util::Graphic::Colors::INVISIBLE,
            Util::Graphic::Colors::INVISIBLE,
            Util::Graphic::Colors::HHU_BLACK,
            Util::Graphic::Colors::INVISIBLE
        },
        {
            Util::Graphic::Colors::INVISIBLE,
            Util::Graphic::Colors::INVISIBLE,
            Util::Graphic::Colors::INVISIBLE,
            Util::Graphic::Colors::HHU_BLACK,
            Util::Graphic::Colors::INVISIBLE
        }
    };

    WidgetStyle buttonStyle = {
        {
            Util::Graphic::Colors::HHU_TURQUOISE.withSaturation(50),
            Util::Graphic::Colors::HHU_TURQUOISE,
            Util::Graphic::Colors::INVISIBLE,
            Util::Graphic::Colors::BLACK,
            Util::Graphic::Colors::HHU_TURQUOISE.withSaturation(50)
        },
        {
            Util::Graphic::Colors::HHU_TURQUOISE.withSaturation(75),
            Util::Graphic::Colors::HHU_TURQUOISE,
            Util::Graphic::Colors::INVISIBLE,
            Util::Graphic::Colors::BLACK,
            Util::Graphic::Colors::HHU_TURQUOISE.withSaturation(75)
        },
        {
            Util::Graphic::Colors::HHU_TURQUOISE,
            Util::Graphic::Colors::HHU_TURQUOISE,
            Util::Graphic::Colors::INVISIBLE,
            Util::Graphic::Colors::BLACK,
            Util::Graphic::Colors::HHU_TURQUOISE
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

#endif
