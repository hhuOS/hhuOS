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
 *
 * The widget and layout system is based on a bachelor's thesis, written by Michael Zuchniewski.
 * The original source code can be found here: https://git.hhu.de/bsinfo/thesis/ba-mizuc100
 */

#ifndef HHUOS_LIB_LUNAR_BUTTON_H
#define HHUOS_LIB_LUNAR_BUTTON_H

#include <stddef.h>

#include "util/base/String.h"
#include "util/graphic/font/Terminal8x8.h"
#include "lunar/Widget.h"

namespace Lunar {

/// A clickable button widget with a single-line text label.
/// To handle button clicks, register an `ActionListener` on the button and listen for mouse clicks.
class Button final : public Widget {

public:
    /// Create a new button instance with the given text and font.
    /// Only a single line of text is supported. If it contains line breaks, only the first line will be displayed.
    explicit Button(const Util::String &text, const Util::Graphic::Font &font = Util::Graphic::Fonts::TERMINAL_8x8) :
        text(text.split("\n")[0]), font(font) {}

    /// Set the text of the button.
    /// Only a single line of text is supported. If it contains line breaks, only the first line will be displayed.
    void setText(const Util::String &text);

    /// Get the text of the button.
    const Util::String& getText() const {
        return text;
    }

    /// Get the preferred width of the button in pixels.
    /// The preferred width is calculated based on the text length and font character width, plus padding.
    size_t getPreferredWidth() const override {
        return font.getCharWidth() * text.length() + 2 * PADDING_X;
    }

    /// Get the preferred height of the button in pixels.
    /// The preferred height is calculated based on the font character height, plus padding.
    size_t getPreferredHeight() const override {
        return font.getCharHeight() + 2 * PADDING_Y;
    }

    /// Draw the button on the given linear frame buffer.
    void draw(const Util::Graphic::LinearFrameBuffer &lfb) override;

private:

    Util::String text;
    const Util::Graphic::Font &font;

    static constexpr size_t PADDING_X = 8;
    static constexpr size_t PADDING_Y= 6;
};

}

#endif