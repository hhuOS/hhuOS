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

#ifndef HHUOS_LIB_LUNAR_INPUTFIELD_H
#define HHUOS_LIB_LUNAR_INPUTFIELD_H

#include <stddef.h>

#include "util/base/String.h"
#include "util/graphic/font/Terminal8x8.h"
#include "lunar/Widget.h"

namespace Lunar {

/// A single-line text input field widget.
/// The user can type text into the input field. The text may be longer than the visible area, in which case it
/// will scroll horizontally. The input field supports backspace for deleting characters,
/// but does not support advanced editing features like cursor movement or text selection.
/// To handle text input, register an `ActionListener` on the input field and listen for key events.
class InputField final : public Widget {

public:
    /// Create a new input field instance with the given width and font.
    explicit InputField(size_t width, const Util::Graphic::Font &font = Util::Graphic::Fonts::TERMINAL_8x8);

    /// Get the text currently entered in the input field.
    const Util::String& getText() const {
        return text;
    }

    /// Get the preferred width of the input field in pixels.
    /// This is the width that was specified during construction or via `setSize()`.
    size_t getPreferredWidth() const override {
        return preferredWidth;
    }

    /// Get the preferred height of the input field in pixels.
    /// The preferred height is calculated based on the font character height, plus padding.
    size_t getPreferredHeight() const override {
        return font.getCharHeight() + PADDING_Y * 2;
    }

    /// Set the size of the input field.
    /// The height will be clamped to the preferred height.
    /// If the new size too small to display at least one character, the input field will not be drawn at all.
    void setSize(size_t width, size_t height) override;

    /// Draw the input field onto the given linear frame buffer.
    void draw(const Util::Graphic::LinearFrameBuffer &lfb) override;

private:

    class KeyInputListener final : public ActionListener {

    public:

        explicit KeyInputListener(InputField &inputField) : inputField(inputField) {}

        void onKeyTyped(const Util::Io::Key &key) override;

    private:

        InputField &inputField;
    };

    Util::String text;
    size_t preferredWidth;
    const Util::Graphic::Font &font;

    static constexpr size_t PADDING_X = 2;
    static constexpr size_t PADDING_Y = 2;
};

}

#endif