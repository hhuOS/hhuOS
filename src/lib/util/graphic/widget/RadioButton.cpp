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
    addActionListener(new ClickListener(*this));
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
        reportSizeChange();
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
    return getHeight() + 2 * PADDING_X + GAP_X + font.getCharWidth() * text.length();
}

size_t RadioButton::getHeight() const {
    return font.getCharHeight() + 2 * PADDING_Y;
}

void RadioButton::draw(const LinearFrameBuffer &lfb) {
    const auto &style = Theme::CURRENT_THEME.radioButton().getStyle(*this);

    const auto diameter = font.getCharHeight();
    const auto radius = diameter / 2;
    const auto posX = getPosX() + PADDING_X;
    const auto posY = getPosY() + PADDING_Y;

    lfb.fillCircle(posX + radius, posY + radius, radius, style.widgetColor);
    lfb.drawCircle(posX + radius, posY + radius, radius, style.borderColor);

    if (selected) {
        lfb.fillCircle(posX + radius, posY + radius, radius / 2, style.accentColor);
    }

    lfb.drawString(font, posX + diameter + GAP_X, posY, static_cast<const char*>(text),
        style.textColor, style.textBackgroundColor);

    Widget::draw(lfb);
}

RadioButton::ClickListener::ClickListener(RadioButton &button) : button(button) {}

void RadioButton::ClickListener::onMouseClicked() {
    button.select();
}

}
