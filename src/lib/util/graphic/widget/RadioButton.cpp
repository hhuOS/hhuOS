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

#include "RadioButton.h"

#include "graphic/widget/RadioButtonGroup.h"

namespace Util::Graphic {

RadioButton::RadioButton(const String &text, const Font &font) : text(text.split("\n")[0]), font(font) {
    addActionListener(new MouseListener(*this));
}

void RadioButton::select() const {
    if (group == nullptr) {
        Panic::fire(Panic::ILLEGAL_STATE, "RadioButton: Not assigned to any group!");
    }

    group->select(groupIndex);
}

void RadioButton::setText(const String &text) {
    const auto newText = text.split("\n")[0];
    if (newText.length() == RadioButton::text.length()) {
        requireRedraw();
    } else {
        requireParentRedraw();
    }

    RadioButton::text = newText;
}

const String& RadioButton::getText() const {
    return text;
}

bool RadioButton::isSelected() const {
    return selected;
}

size_t RadioButton::getWidth() const {
    return getHeight() + style.gapX + font.getCharWidth() * text.length();
}

size_t RadioButton::getHeight() const {
    return font.getCharHeight();
}

void RadioButton::draw(const LinearFrameBuffer &lfb) {
    const auto &backgroundColor = hovered ? style.backgroundColorHighlighted : style.backgroundColor;
    const auto height = getHeight();
    const auto radius = height / 2;
    const auto posX = getPosX();
    const auto posY = getPosY();

    lfb.fillCircle(posX + radius, posY + radius, radius, pressed ? backgroundColor.dim() : backgroundColor);
    lfb.drawCircle(posX + radius, posY + radius, radius, style.borderColor);

    if (selected) {
        lfb.fillCircle(posX + radius, posY + radius, radius / 2, hovered ? style.borderColor.dim() : style.borderColor);
    }

    lfb.drawString(font, posX + height + style.gapX, posY, static_cast<const char*>(text),
        style.textColor, Colors::INVISIBLE);

    Widget::draw(lfb);
}

RadioButton::MouseListener::MouseListener(RadioButton &button) : button(button) {}

void RadioButton::MouseListener::onMouseEntered() {
    button.hovered = true;
    button.requireRedraw();
}

void RadioButton::MouseListener::onMouseExited() {
    button.hovered = false;
    button.requireRedraw();
}

void RadioButton::MouseListener::onMousePressed() {
    button.pressed = true;
    button.requireRedraw();
}

void RadioButton::MouseListener::onMouseReleased() {
    button.pressed = false;
    button.select();
    button.requireRedraw();
}

}
