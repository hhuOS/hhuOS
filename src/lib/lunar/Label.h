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

#ifndef HHUOS_LIB_LUNAR_LABEL_H
#define HHUOS_LIB_LUNAR_LABEL_H

#include <stddef.h>

#include "util/base/String.h"
#include "util/graphic/font/Terminal8x8.h"
#include "util/graphic/LinearFrameBuffer.h"
#include "lunar/Widget.h"

namespace Lunar {

/// A simple widget that displays a text label.
/// The label can have a maximum width, causing the text to wrap into multiple lines if necessary.
/// Line breaks can also be inserted manually using the newline character '\n'.
class Label final : public Widget {

public:
    /// Create a new label instance with the given text, maximum width and font.
    /// The text will automatically be wrapped into multiple lines if it exceeds the maximum width.
    /// A maximum width of 0 means no maximum width (and no automatic wrapping).
    Label(const Util::String &text, const size_t maxWidth,
        const Util::Graphic::Font &font = Util::Graphic::Fonts::TERMINAL_8x8) :
        text(text), maxWidth(maxWidth), font(font), preferredLines(calculateLines(text, maxWidth, font)) {}

    /// Create a new label instance with the given text and font a maximum width of 0 (no automatic wrapping).
    explicit Label(const Util::String &text, const Util::Graphic::Font &font = Util::Graphic::Fonts::TERMINAL_8x8) :
        Label(text, 0, font) {}

    /// Set the text of the label.
    /// The text will be automatically wrapped into multiple lines if it exceeds the maximum width.
    void setText(const Util::String &text);

    /// Get the text of the label.
    Util::String getText() const {
        return text;
    }

    /// Get the preferred width of the label.
    /// The preferred width is the maximum width of the longest line in the label.
    size_t getPreferredWidth() const override;

    /// Get the preferred height of the label.
    /// The preferred height is the number of lines in the label multiplied by the height of the font.
    size_t getPreferredHeight() const override {
        return preferredLines.length() * font.getCharHeight();
    }

    /// Set the size of the label.
    /// If the new size is larger than the preferred size, it will be clamped to the preferred size.
    /// This way, the label will never be larger than necessary to display its content.
    void setSize(size_t width, size_t height) override;

    /// Draw the label on the given linear frame buffer.
    void draw(const Util::Graphic::LinearFrameBuffer &lfb) override;

private:

    static Util::Array<Util::String> calculateLines(const Util::String &text, size_t maxWidth, const Util::Graphic::Font &font);

    Util::String text;
    size_t maxWidth;
    const Util::Graphic::Font &font;

    Util::Array<Util::String> lines;
    Util::Array<Util::String> preferredLines;
};

} // namespace Util

#endif
