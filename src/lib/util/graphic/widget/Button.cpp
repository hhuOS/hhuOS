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

namespace Util::Graphic {

Button::Button(const String &text, const Font &font) : text(text.split("\n")[0]), font(font) {
    addActionListener(new MouseHoverListener(*this));
}

size_t Button::getWidth() const {
    return font.getCharWidth() * text.length() + 2 * style.paddingX;
}

size_t Button::getHeight() const {
    return font.getCharHeight() + 2 * style.paddingY;
}

void Button::setText(const String &text) {
    Button::text = text;
    requireParentRedraw();
}

void Button::draw(LinearFrameBuffer &lfb) {
    const auto &backgroundColor = pressed ? style.backgroundColorHighlighted : style.backgroundColor;
    const auto &textColor = hovered || pressed ? style.textColorHighlighted : style.textColor;
    const auto width = getWidth();
    const auto height = getHeight();
    const auto posX = getPosX();
    const auto posY = getPosY();

    // Draw button area
    lfb.fillRectangle(posX, posY, width, height, backgroundColor);
    lfb.drawRectangle(posX, posY, width, height, style.borderColor);

    // Calculate centered text position
    const auto textWidth = text.length() * font.getCharWidth();
    const auto innerWidth = width - style.paddingX * 2;
    const auto textX = posX + style.paddingX + (innerWidth  - textWidth) / 2;
    const auto textY = posY + style.paddingY + (height - style.paddingY - font.getCharHeight()) / 2;

    // Draw button
    lfb.drawString(font, textX, textY, static_cast<const char*>(text), textColor, backgroundColor);

    Widget::draw(lfb);
}

Button::MouseHoverListener::MouseHoverListener(Button &button) : button(button) {}

void Button::MouseHoverListener::onMouseEnter() {
    button.hovered = true;
    button.requireRedraw();
}

void Button::MouseHoverListener::onMouseLeave() {
    button.hovered = false;
    button.requireRedraw();
}

void Button::MouseHoverListener::onMousePress() {
    button.pressed = true;
    button.requireRedraw();
}

void Button::MouseHoverListener::onMouseRelease() {
    button.pressed = false;
    button.requireRedraw();
}

}
