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

#ifndef HHUOS_LIB_LUNAR_RADIOBUTTON_H
#define HHUOS_LIB_LUNAR_RADIOBUTTON_H

#include "util/base/String.h"
#include "util/graphic/font/Terminal8x8.h"
#include "lunar/Widget.h"

namespace Lunar {

class RadioButtonGroup;

/// A radio button widget with a text label.
/// Radio buttons are used to allow the user to select one option from a set of mutually exclusive options.
/// Radio buttons are typically grouped together in a `RadioButtonGroup`, which ensures that only one
/// radio button in the group can be selected at a time.
/// To handle radio button selection changes, register an `ActionListener` on the radio button
/// and listen for mouse clicks.
class RadioButton final : public Widget {

public:
    /// Create a new radio button instance with the given text and font.
    /// Only a single line of text is supported. If it contains line breaks, only the first line will be displayed.
    explicit RadioButton(const Util::String &text,
        const Util::Graphic::Font &font = Util::Graphic::Fonts::TERMINAL_8x8);

    /// Select this radio button.
    /// This will automatically deselect any other radio button in the same group.
    /// If the radio button is already selected, this method has no effect.
    /// If the radio button is not part of a group, a panic is fired.
    void select() const;

    /// Check whether the radio button is currently selected.
    bool isSelected() const {
        return selected;
    }

    /// Set the text of the radio button.
    /// Only a single line of text is supported. If it contains line breaks, only the first line will be displayed.
    void setText(const Util::String &text);

    /// Get the text of the radio button.
    const Util::String& getText() const {
        return text;
    }

    /// Get the preferred width of the radio button in pixels.
    /// The preferred width is calculated based on the text length and font character width,
    /// plus space for the radio button.
    size_t getPreferredWidth() const override {
        return getPreferredHeight() + GAP_X + font.getCharWidth() * text.length();
    }

    /// Get the preferred height of the radio button in pixels.
    /// The preferred height is calculated based on the font character height.
    size_t getPreferredHeight() const override {
        return font.getCharHeight();
    }

    /// Set the size of the radio button.
    /// If the new size is larger than the preferred size, it will be clamped to the preferred size.
    /// This way, the radio button will never be larger than necessary to display its content.
    /// If the new width is smaller than the preferred width, the text will be truncated.
    /// If the new size is too small to display the widget in a reasonable way, it will not be drawn at all.
    void setSize(size_t width, size_t height) override;

    /// Draw the radio button on the given linear frame buffer.
    void draw(const Util::Graphic::LinearFrameBuffer &lfb) override;

private:

    friend class RadioButtonGroup;

    class ClickListener final : public ActionListener {

    public:

        explicit ClickListener(RadioButton &button) : button(button) {}

        void onMouseClicked() override {
            button.select();
        }

    private:

        RadioButton &button;
    };

    Util::String text;
    const Util::Graphic::Font &font;

    RadioButtonGroup *group = nullptr;
    int32_t groupIndex = -1;

    bool selected = false;

    static constexpr size_t GAP_X = 6;
};

}

#endif