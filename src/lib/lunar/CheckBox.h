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

#ifndef HHUOS_LIB_LUNAR_CHECKBOX_H
#define HHUOS_LIB_LUNAR_CHECKBOX_H

#include "util/base/String.h"
#include "util/graphic/font/Terminal8x8.h"
#include "lunar/Widget.h"

namespace Lunar {

/// A checkbox widget with a text label.
/// The checkbox can be checked or unchecked by clicking on it.
/// To handle checkbox state changes, register an `ActionListener` on the checkbox and listen for mouse clicks.
class CheckBox final : public Widget {

public:
    /// Create a new checkbox instance with the given text and font.
    /// Only a single line of text is supported. If it contains line breaks, only the first line will be displayed.
    explicit CheckBox(const Util::String &text, const Util::Graphic::Font &font = Util::Graphic::Fonts::TERMINAL_8x8);

    /// Toggle the checked state of the checkbox.
    void toggle() {
        checked = !checked;
        requireRedraw();
    }

    /// Set the text of the checkbox.
    /// Only a single line of text is supported. If it contains line breaks, only the first line will be displayed.
    void setText(const Util::String &text);

    /// Get the text of the checkbox.
    const Util::String& getText() const {
        return text;
    }

    /// Check whether the checkbox is currently checked.
    bool isChecked() const {
        return checked;
    }

    /// Get the preferred width of the checkbox in pixels.
    /// The preferred width is calculated based on the text length and font character width,
    /// plus space for the checkbox.
    size_t getPreferredWidth() const override {
        return getPreferredHeight() + GAP_X + font.getCharWidth() * text.length();
    }

    /// Get the preferred height of the checkbox in pixels.
    /// The preferred height is calculated based on the font character height.
    size_t getPreferredHeight() const override {
        return font.getCharHeight();
    }

    /// Set the size of the checkbox.
    /// If the new size is larger than the preferred size, it will be clamped to the preferred size.
    /// This way, the checkbox will never be larger than necessary to display its content.
    /// If the new width is smaller than the preferred width, the text will be truncated.
    /// If the new size is too small to display the widget in a reasonable way, it will not be drawn at all.
    void setSize(size_t width, size_t height) override;

    /// Draw the checkbox on the given linear frame buffer.
    void draw(const Util::Graphic::LinearFrameBuffer &lfb) override;

private:

    class ClickListener final : public ActionListener {

    public:

        explicit ClickListener(CheckBox &box) : box(box) {}

        void onMouseClicked() override {
            box.toggle();
        }

    private:

        CheckBox &box;
    };

    Util::String text;
    const Util::Graphic::Font &font;

    bool checked = false;

    bool hovered = false;
    bool pressed = false;

    static constexpr size_t GAP_X = 6;
};

}

#endif