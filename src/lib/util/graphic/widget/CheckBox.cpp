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

#include "CheckBox.h"

namespace Util::Graphic {

CheckBox::CheckBox(const String &text, const Font &font) : text(text.split("\n")[0]), font(font) {
    addActionListener(new MouseListener(*this));
}

void CheckBox::toggle() {
    checked = !checked;
    requireRedraw();
}

void CheckBox::setText(const String &text) {
    const auto newText = text.split("\n")[0];
    if (newText.length() == CheckBox::text.length()) {
        requireRedraw();
    } else {
        requireParentRedraw();
    }

    CheckBox::text = newText;
}

bool CheckBox::isChecked() const {
    return checked;
}

size_t CheckBox::getWidth() const {
    return getHeight() + style.gapX + font.getCharWidth() * text.length();
}

size_t CheckBox::getHeight() const {
    return font.getCharHeight();
}

void CheckBox::draw(const LinearFrameBuffer &lfb) {
    const auto &backgroundColor = hovered ? style.backgroundColorHighlighted : style.backgroundColor;
    const auto height = getHeight();
    const auto posX = getPosX();
    const auto posY = getPosY();

    // Draw box
    lfb.fillSquare(posX, posY, height, pressed ? backgroundColor.dim() : backgroundColor);
    lfb.drawSquare(posX, posY, height, style.borderColor);

    // Draw checkmark (if checked)
    if (checked) {
        const auto inset = height / 6;
        const auto leftX = posX + inset;
        const auto midY = posY + height / 2;
        const auto midX = posX + height / 2;
        const auto bottomY = posY + height - inset;
        const auto rightX = posX + height - inset;
        const auto topY = posY + inset;
        lfb.drawLine(leftX, midY, midX, bottomY, style.accentColor);
        lfb.drawLine(midX, bottomY, rightX, topY, style.accentColor);
    }

    // Calculate text position
    const auto textX = posX + height + style.gapX;
    const auto textY = posY + (height - font.getCharHeight()) / 2;

    // Draw text
    lfb.drawString(font, textX, textY, static_cast<const char*>(text), style.textColor, Colors::INVISIBLE);
}

CheckBox::MouseListener::MouseListener(CheckBox &box) : box(box) {}

void CheckBox::MouseListener::onMouseEnter() {
    box.hovered = true;
    box.requireRedraw();
}

void CheckBox::MouseListener::onMouseLeave() {
    box.hovered = false;
    box.requireRedraw();
}

void CheckBox::MouseListener::onMousePress() {
    box.pressed = true;
    box.requireRedraw();
}

void CheckBox::MouseListener::onMouseRelease() {
    box.pressed = false;
    box.toggle();
}

}
