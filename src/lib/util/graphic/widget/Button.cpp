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

#include "Button.h"

#include "graphic/widget/Theme.h"

namespace Util::Graphic {

Button::Button(const String &text, const Font &font) : text(text.split("\n")[0]), font(font) {}

void Button::setText(const String &text) {
    const auto newText = text.split("\n")[0];
    if (newText.length() == Button::text.length()) {
        requireRedraw();
    } else {
        reportPreferredSizeChange();
    }

    Button::text = newText;
}

const String& Button::getText() const {
    return text;
}

size_t Button::getPreferredWidth() const {
    return font.getCharWidth() * text.length() + 2 * PADDING_X;
}

size_t Button::getPreferredHeight() const {
    return font.getCharHeight() + 2 * PADDING_Y;
}

void Button::draw(const LinearFrameBuffer &lfb) {
    const auto &style = Theme::CURRENT_THEME.button().getStyle(*this);

    const auto width = getWidth();
    const auto height = getHeight();
    const auto posX = getPosX();
    const auto posY = getPosY();

    // Draw button area
    lfb.fillRectangle(posX, posY, width, height, style.widgetColor);
    lfb.drawRectangle(posX, posY, width, height, style.borderColor);

    const auto textHeight = font.getCharHeight();
    if (height < textHeight) {
        // Not enough space to draw text
        return;
    }

    // Calculate maximum text length that fits into the button
    const auto maxTextWidth = width - PADDING_X * 2;
    const auto maxTextLength = maxTextWidth / font.getCharWidth();

    auto text = Button::text;
    if (text.length() > maxTextLength) {
        // Not enough space to draw full text, truncate it and add "..."
        if (maxTextLength < 3) {
            // Not enough space to even draw "..."
            Widget::draw(lfb);
            return;
        }

        text = text.substring(0, maxTextLength - 3) + "...";
    }

    // Calculate centered text position
    const auto textWidth = text.length() * font.getCharWidth();
    const auto textX = posX + (width - textWidth) / 2;
    const auto textY = posY + (height - font.getCharHeight()) / 2;

    // Draw button
    lfb.drawString(font, textX, textY, static_cast<const char*>(text),
        style.textColor, style.textBackgroundColor);

    Widget::draw(lfb);
}

}
